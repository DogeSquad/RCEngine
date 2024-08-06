#pragma once

#include "rce_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace rce
{

class RCEModel {
public:

	struct Vertex {
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};
	RCEModel(RCEDevice& device, const std::vector<Vertex> &vertices);
	~RCEModel();

	RCEModel(const RCEModel&) = delete;
	RCEModel& operator=(const RCEModel&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	RCEDevice& rceDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	uint32_t vertexCount;

	void createVertexBuffer(const std::vector<Vertex>& vertices);
};

}