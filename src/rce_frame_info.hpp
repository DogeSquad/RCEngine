#pragma once

#include "rce_camera.hpp"
#include "rce_object.hpp"

#include <vulkan/vulkan.h>

namespace rce
{

constexpr auto MAX_LIGHTS = 10;

struct PointLight 
{
	glm::vec4 position{};
	glm::vec4 color{};
};

struct GlobalUbo {
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 inverseView{ 1.f };
	glm::vec4 ambientColor{ 1.f, 1.f, 1.f, 0.02f };
	PointLight pointLights[MAX_LIGHTS];
	int numLights;
};

struct FrameInfo 
{
	int frameIndex;
	float time;
	VkCommandBuffer commandBuffer;
	RCECamera& camera;
	VkDescriptorSet globalDescriptorSet;
	RCEObject::Map& objects;
};

}