#pragma once

// Design inspired by Brendan Galea's YouTube tutorials.

#include "rce_window.hpp"
#include "rce_pipeline.hpp"
#include "rce_device.hpp"
#include "rce_swap_chain.hpp"

#include <memory>
#include <vector>

namespace rce {
	class RCEngine {
	public:
		static constexpr int WIDTH = 1200;
		static constexpr int HEIGHT = 800;

		RCEngine();
		~RCEngine();

		RCEngine(const RCEngine&) = delete;
		RCEngine& operator=(const RCEngine&) = delete;

		void run();
	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		RCEWindow rceWindow{ WIDTH, HEIGHT, "RCEngine" };
		RCEDevice rceDevice{ rceWindow };
		RCESwapChain rceSwapChain{ rceDevice, rceWindow.getExtent()};
		std::unique_ptr<RCEPipeline> rcePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}