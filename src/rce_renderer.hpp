#pragma once

// Design inspired by Brendan Galea's YouTube tutorials.

#include "rce_window.hpp"
#include "rce_device.hpp"
#include "rce_swap_chain.hpp"
#include "rce_model.hpp"

#include <memory>
#include <vector>
#include <cassert>

namespace rce {
	class RCERenderer {
	public:
		RCERenderer(RCEWindow& window, RCEDevice& device);
		~RCERenderer();

		RCERenderer(const RCERenderer&) = delete;
		RCERenderer& operator=(const RCERenderer&) = delete;

		VkRenderPass getSwapChainRenderPass() const { return rceSwapChain->getRenderPass(); }
		float getAspectRatio() const { return rceSwapChain->extentAspectRatio(); }
		bool isFrameInProgress() const { return isFrameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const {
			assert(isFrameStarted && "Cannot get command buffer when frame in progress!");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const {
			assert(isFrameStarted && "Cannot get frame index when frame not in progress!");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();

		RCEWindow& rceWindow;
		RCEDevice& rceDevice;
		std::unique_ptr<RCESwapChain> rceSwapChain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex;
		int currentFrameIndex{ 0 };
		bool isFrameStarted{ false };

	};
}