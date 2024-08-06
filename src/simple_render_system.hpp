#pragma once

// Design inspired by Brendan Galea's YouTube tutorials.

#include "rce_pipeline.hpp"
#include "rce_device.hpp"
#include "rce_model.hpp"
#include "rce_object.hpp"

#include <memory>
#include <vector>

namespace rce {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(RCEDevice& device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderObjects(VkCommandBuffer commandBuffer, std::vector<RCEObject>& objects);
	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		RCEDevice& rceDevice;

		std::unique_ptr<RCEPipeline> rcePipeline;
		VkPipelineLayout pipelineLayout;
	};
}