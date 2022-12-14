#ifndef SVE_MODEL_H
#define SVE_MODEL_H

#include "SVEDevice.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

class SVEModel
{
public:
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 color;

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
	};

	SVEModel(SVEDevice& device, const std::vector<Vertex>& vertices);
	~SVEModel();

	SVEModel(const SVEModel&) = delete;
	SVEModel& operator=(const SVEModel&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);

	SVEDevice& sveDevice;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	uint32_t vertexCount;
};

#endif