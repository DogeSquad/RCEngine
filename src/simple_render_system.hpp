#pragma once

// Design inspired by Brendan Galea's YouTube tutorials.

#include "rce_pipeline.hpp"
#include "rce_device.hpp"
#include "rce_model.hpp"
#include "rce_object.hpp"
#include "rce_camera.hpp"
#include "rce_frame_info.hpp"

#include <memory>
#include <vector>

namespace rce {
	class SimpleRenderSystem {
	public:
		SimpleRenderSystem(RCEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderObjects(
			FrameInfo& frameInfo, 
			std::vector<RCEObject>& objects);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		RCEDevice& rceDevice;

		std::unique_ptr<RCEPipeline> rcePipeline;
		VkPipelineLayout pipelineLayout;
	};
}