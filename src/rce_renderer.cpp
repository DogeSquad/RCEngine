#include "rce_renderer.hpp"

#include <stdexcept>
#include <array>
namespace rce {

RCERenderer::RCERenderer(RCEWindow& window, RCEDevice& device) :
	rceWindow{window},
	rceDevice{device}
{
	recreateSwapChain();
	createCommandBuffers();
}
RCERenderer::~RCERenderer()
{
	freeCommandBuffers();
}

VkCommandBuffer RCERenderer::beginFrame()
{
	assert(!isFrameStarted && "Cannot call beginFrame while frame already in progress!");
	auto result = rceSwapChain->acquireNextImage(&currentImageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		throw std::runtime_error("failed to acquire swapchain image.");

	isFrameStarted = true;

	auto commandBuffer = getCurrentCommandBuffer();
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		throw std::runtime_error("failed to begin command buffer.");

	return commandBuffer;
}

void RCERenderer::endFrame()
{
	assert(isFrameStarted && "Cannot call endFrame while frame is not in progress!");
	
	auto commandBuffer = getCurrentCommandBuffer();
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to end command buffer recording");

	auto result = rceSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || rceWindow.wasWindowResized()) {
		rceWindow.resetWindowResizedFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
		throw std::runtime_error("failed to submit to swap chain.");

	isFrameStarted = false;
	currentFrameIndex = (currentFrameIndex + 1) % RCESwapChain::MAX_FRAMES_IN_FLIGHT;
}

void RCERenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Cannot call beginSwapChainRenderPass if the frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cant begin Render Pass on command buffer from different frame!");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = rceSwapChain->getRenderPass();
	renderPassInfo.framebuffer = rceSwapChain->getFrameBuffer(currentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = rceSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = static_cast<float>(rceSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(rceSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0,}, rceSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

}

void RCERenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Cannot call endSwapChainRenderPass if the frame is not in progress!");
	assert(commandBuffer == getCurrentCommandBuffer() && "Cant end Render Pass on command buffer from different frame!");

	vkCmdEndRenderPass(commandBuffer);
}

void RCERenderer::createCommandBuffers()
{
	commandBuffers.resize(RCESwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = rceDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(rceDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		throw std::runtime_error("failed to allocate command buffers.");
}

void RCERenderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(
		rceDevice.device(),
		rceDevice.getCommandPool(),
		static_cast<uint32_t>(commandBuffers.size()),
		commandBuffers.data());
	commandBuffers.clear();
}

void RCERenderer::recreateSwapChain()
{
	auto extent = rceWindow.getExtent();
	while (extent.width == 0 || extent.height == 0) {
		extent = rceWindow.getExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(rceDevice.device());
	//rceSwapChain = nullptr;

	if (rceSwapChain == nullptr)
		rceSwapChain = std::make_unique<RCESwapChain>(rceDevice, extent);
	else {
		std::shared_ptr<RCESwapChain> oldSwapChain = std::move(rceSwapChain);
		rceSwapChain = std::make_unique<RCESwapChain>(rceDevice, extent, oldSwapChain);

		if (!oldSwapChain->compareSwapFormats(*rceSwapChain.get()))
			throw std::runtime_error("Swap chain image (or depth) format has changed!");
	}

	// TODO
}

};

