#pragma once

#include "rce_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace rce
{

class RCEModel 
{
public:

	struct Vertex 
	{
		glm::vec3 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};
	struct Builder
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};
	};

	RCEModel(RCEDevice& device, const RCEModel::Builder &builder);
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

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	uint32_t indexCount;
	bool hasIndexBuffer = false;

	void createVertexBuffer(const std::vector<Vertex>& vertices);
	void createIndexBuffer(const std::vector<uint32_t>& indices);
};

}