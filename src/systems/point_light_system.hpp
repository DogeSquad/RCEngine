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
#include <map>

namespace rce {
	class PointLightSystem {
	public:
		PointLightSystem(RCEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frameInfo, GlobalUbo& ubo);
		void render(FrameInfo& frameInfo);
	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		RCEDevice& rceDevice;

		std::unique_ptr<RCEPipeline> rcePipeline;
		VkPipelineLayout pipelineLayout;
	};
}