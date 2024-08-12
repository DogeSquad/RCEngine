#include "rcengine.hpp"

#include "systems/simple_render_system.hpp"
#include "systems/point_light_system.hpp"
#include "rce_camera.hpp"
#include "keyboard_movement_controller.hpp"
#include "rce_buffer.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace rce {


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // ESC to close app
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, GLFW_TRUE);

    // F1 to toggle between Maximize and Restore
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) glfwGetWindowAttrib(window, GLFW_MAXIMIZED) == GLFW_TRUE ? glfwRestoreWindow(window) : glfwMaximizeWindow(window);
}

RCEngine::RCEngine() { 
    globalPool = RCEDescriptorPool::Builder(rceDevice)
        .setMaxSets(RCESwapChain::MAX_FRAMES_IN_FLIGHT)
        .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, RCESwapChain::MAX_FRAMES_IN_FLIGHT)
        .build();
    loadObjects();

    // Set up window icon
    GLFWimage images[2];
    images[0].pixels = stbi_load("../osprey_engine_lowres.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
    images[1].pixels = stbi_load("../osprey_engine.png", &images[1].width, &images[1].height, 0, 4); //rgba channels 
    glfwSetWindowIcon(rceWindow.getGLFWwindow(), 2, images);
    stbi_image_free(images[0].pixels);
    stbi_image_free(images[1].pixels);

    glfwSetKeyCallback(rceWindow.getGLFWwindow(), key_callback);
}

RCEngine::~RCEngine() {}

void RCEngine::run()
{
    std::vector<std::unique_ptr<RCEBuffer>> uboBuffers(RCESwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
        uboBuffers[i] = std::make_unique<RCEBuffer>(
            rceDevice,
            sizeof(GlobalUbo),
            1,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        );
        uboBuffers[i]->map();
    }

    auto globalSetLayout = RCEDescriptorSetLayout::Builder(rceDevice)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
        .build();

    std::vector<VkDescriptorSet> globalDescriptorSets(RCESwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = uboBuffers[i]->descriptorInfo();
        RCEDescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

	SimpleRenderSystem simpleRenderSystem{ rceDevice, rceRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    PointLightSystem pointLightSystem{ rceDevice, rceRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
    RCECamera camera{};
    //camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .5f, 0.f, 1.f });
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = RCEObject::createObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!rceWindow.shouldClose()) {
		glfwPollEvents();

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(rceWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = rceRenderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, -1.f, 1.f);
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

		if (auto commandBuffer = rceRenderer.beginFrame()) {
            int frameIndex = rceRenderer.getFrameIndex();
            FrameInfo frameInfo{
                frameIndex,
                frameTime,
                commandBuffer,
                camera,
                globalDescriptorSets[frameIndex],
                objects
            };
            // prepare & update
            GlobalUbo ubo{};
            ubo.projection = camera.getProjection();
            ubo.view = camera.getView();
            ubo.inverseView = camera.getInverseView();
            pointLightSystem.update(frameInfo, ubo);
            uboBuffers[frameIndex]->writeToBuffer(&ubo);
            uboBuffers[frameIndex]->flush();

            // render
			rceRenderer.beginSwapChainRenderPass(commandBuffer);

			simpleRenderSystem.renderObjects(frameInfo);
            pointLightSystem.render(frameInfo);

			rceRenderer.endSwapChainRenderPass(commandBuffer);
			rceRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(rceDevice.device());
}

void RCEngine::loadObjects()
{    
    // Loading smooth vase
    std::shared_ptr<RCEModel> smoothVaseModel = RCEModel::createModelFromFile(rceDevice, "../models/smooth_vase.obj");
    auto smoothVase = RCEObject::createObject();
    smoothVase.model = smoothVaseModel;
    smoothVase.transform.translation = { -.5f, -1, 0.f };
    smoothVase.transform.rotation = glm::normalize(glm::vec3(-1.f, 2.5f, 2.f));
    smoothVase.transform.scale = glm::vec3{ 3.f };
    objects.emplace(smoothVase.getId(), std::move(smoothVase));

    // Loading flat vase
    std::shared_ptr<RCEModel> flatVaseModel = RCEModel::createModelFromFile(rceDevice, "../models/flat_vase.obj");
    auto flatVase = RCEObject::createObject();
    flatVase.model = flatVaseModel;
    flatVase.transform.translation = { .5f, 0.f, 0.f };
    flatVase.transform.scale = glm::vec3{ 3.f };
    objects.emplace(flatVase.getId(), std::move(flatVase));

    // Loading ground
    std::shared_ptr<RCEModel> floorModel = RCEModel::createModelFromFile(rceDevice, "../models/quad.obj");
    auto floor = RCEObject::createObject();
    floor.model = floorModel;
    floor.transform.translation = { 0.f, 0.f, 0.f };
    floor.transform.scale = glm::vec3{ 2.f , 1.f, 2.f};
    objects.emplace(floor.getId(), std::move(floor));

    std::vector<glm::vec3> lightColors{
        {1.f, .1f, .1f},
        {.1f, .1f, 1.f},
        {.1f, 1.f, .1f},
        {1.f, 1.f, .1f},
        {.1f, 1.f, 1.f},
        {1.f, 1.f, 1.f}  //
    };

    for (int i = 0; i < lightColors.size(); i++) {
        auto pointLight = RCEObject::makePointLight(.2f);
        pointLight.color = lightColors[i];
        auto rotateLight = glm::rotate(
            glm::mat4(1.f),
            (i * glm::two_pi<float>()) / lightColors.size(),
            { 0.f, -1.f, 0.f });
        pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-1.f, -1.f, -1.f, 1.f));
        objects.emplace(pointLight.getId(), std::move(pointLight));
    }
}

};

