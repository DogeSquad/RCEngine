#include "rcengine.hpp"

#include <stdexcept>
#include <array>

namespace rce {

RCEngine::RCEngine()
{
	loadModels();
	createPipelineLayout();
	createPipeline();
	createCommandBuffers();
}
RCEngine::~RCEngine()
{
	vkDestroyPipelineLayout(rceDevice.device(), pipelineLayout, nullptr);
}

void RCEngine::run()
{
	while (!rceWindow.shouldClose()) {
		glfwPollEvents();
		drawFrame();
	}

	vkDeviceWaitIdle(rceDevice.device());
}

void RCEngine::loadModels()
{
	std::vector<RCEModel::Vertex> vertices{
		{{ 0.0f, -0.5f}},
		{{ 0.5f,  0.5f}},
		{{-0.5f,  0.5f}}
	};

	rceModel = std::make_unique<RCEModel>(rceDevice, vertices);
}

void RCEngine::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(rceDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		throw std::runtime_error("failed to create pipeline layout.");
}

void RCEngine::createPipeline()
{
	PipelineConfigInfo pipelineConfig{};
	RCEPipeline::defaultPipelineConfigInfo(pipelineConfig, rceSwapChain.width(), rceSwapChain.height());
	pipelineConfig.renderPass = rceSwapChain.getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	rcePipeline = std::make_unique<RCEPipeline>(
		rceDevice,
		"../shaders/simple_shader.vert.spv",
		"../shaders/simple_shader.frag.spv",
		pipelineConfig
	);
}

void RCEngine::createCommandBuffers()
{
	commandBuffers.resize(rceSwapChain.imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = rceDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(rceDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers.");

	for (int i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			throw std::runtime_error("failed to begin command buffer.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = rceSwapChain.getRenderPass();
		renderPassInfo.framebuffer = rceSwapChain.getFrameBuffer(i);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = rceSwapChain.getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		rcePipeline->bind(commandBuffers[i]);
		rceModel->bind(commandBuffers[i]);
		rceModel->draw(commandBuffers[i]);

		vkCmdEndRenderPass(commandBuffers[i]);
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			throw std::runtime_error("failed to end command buffer recording");
	}
}

void RCEngine::drawFrame()
{
	uint32_t imageIndex;
	auto result = rceSwapChain.acquireNextImage(&imageIndex);

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("failed to acquire swapchain image.");

	result = rceSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS)
		throw std::runtime_error("failed to submit to swap chain.");
}

};

