#pragma once

#include "rce_camera.hpp"

#include <vulkan/vulkan.h>

namespace rce
{

struct FrameInfo {
	int frameIndex;
	float time;
	VkCommandBuffer commandBuffer;
	RCECamera& camera;
	VkDescriptorSet globalDescriptorSet;
};

}