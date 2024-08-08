#pragma once

#include "rce_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace rce
{

class RCEModel 
{
public:

	struct Vertex 
	{
		glm::vec3 position{};
		glm::vec3 color{};
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const {
			return position == other.position 
				&& color == other.color 
				&& normal == other.normal 
				&& uv == other.uv;
		}
	};
	struct Builder
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string& filepath);
	};

	RCEModel(RCEDevice& device, const RCEModel::Builder &builder);
	~RCEModel();

	RCEModel(const RCEModel&) = delete;
	RCEModel& operator=(const RCEModel&) = delete;

	static std::unique_ptr<RCEModel> createModelFromFile(RCEDevice& device, const std::string& filepath);

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