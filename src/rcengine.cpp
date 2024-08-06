#include "rcengine.hpp"

#include "simple_render_system.hpp"

#include <stdexcept>
#include <array>

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

	while (!rceWindow.shouldClose()) {
		glfwPollEvents();

		if (auto commandBuffer = rceRenderer.beginFrame()) {
			rceRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderObjects(commandBuffer, objects);
			rceRenderer.endSwapChainRenderPass(commandBuffer);
			rceRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(rceDevice.device());
}

void RCEngine::loadObjects()
{
	std::vector<RCEModel::Vertex> vertices{
		{{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
	};

	auto rceModel = std::make_shared<RCEModel>(rceDevice, vertices);

	auto triangle = RCEObject::createObject();
	triangle.model = rceModel;
	triangle.color = { 0.1f, 0.8f, 0.1f };
	triangle.transform2d.translation.x = 0.2f;
	triangle.transform2d.scale = { 1.0f, 1.0f };
	triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

	objects.push_back(std::move(triangle));
}

};

