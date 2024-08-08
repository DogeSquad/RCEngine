#include "rcengine.hpp"

#include "simple_render_system.hpp"
#include "rce_camera.hpp"
#include "keyboard_movement_controller.hpp"

#include <stdexcept>
#include <array>
#include <chrono>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace rce {

RCEngine::RCEngine() { loadObjects(); }

RCEngine::~RCEngine() {}

void RCEngine::run()
{
	SimpleRenderSystem simpleRenderSystem{ rceDevice, rceRenderer.getSwapChainRenderPass() };
    RCECamera camera{};
    //camera.setViewDirection(glm::vec3{ 0.f }, glm::vec3{ .5f, 0.f, 1.f });
    camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

    auto viewerObject = RCEObject::createObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

	while (!rceWindow.shouldClose()) {
		glfwPollEvents();

        // ESC to close app
        if (glfwGetKey(rceWindow.getGLFWwindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(rceWindow.getGLFWwindow(), GLFW_TRUE);

        auto newTime = std::chrono::high_resolution_clock::now();
        float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
        currentTime = newTime;

        cameraController.moveInPlaneXZ(rceWindow.getGLFWwindow(), frameTime, viewerObject);
        camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

        float aspect = rceRenderer.getAspectRatio();
        //camera.setOrthographicProjection(-aspect, aspect, -1.f, 1.f, -1.f, 1.f);
        camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

		if (auto commandBuffer = rceRenderer.beginFrame()) {
			rceRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderObjects(commandBuffer, objects, camera);
			rceRenderer.endSwapChainRenderPass(commandBuffer);
			rceRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(rceDevice.device());
}

void RCEngine::loadObjects()
{
    //std::shared_ptr<RCEModel> rceModel = RCEModel::createModelFromFile(rceDevice, "../models/colored_cube.obj");
    std::shared_ptr<RCEModel> rceModel = RCEModel::createModelFromFile(rceDevice, "../models/smooth_vase.obj");

    auto obj = RCEObject::createObject();
    obj.model = rceModel;
    obj.transform.translation = { 0.f, 0.f, 2.5f };
    obj.transform.scale = glm::vec3{3.f};
    objects.push_back(std::move(obj));
}

};

