#ifndef SVE_MODEL_H
#define SVE_MODEL_H

#include "SVEDevice.h"
#include "SVEBuffer.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

class SVEModel
{
public:
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
		glm::vec3 normal{};
		glm::vec2 uv{};

		static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

		bool operator==(const Vertex& other) const
		{
			return position == other.position && 
				color == other.color && 
				normal == other.normal &&
				uv == other.uv;
		}
	};

	struct Builder
	{
		std::vector<Vertex> vertices{};
		std::vector<uint32_t> indices{};

		void loadModel(const std::string& filepath);
	};

	SVEModel(SVEDevice& device, const SVEModel::Builder& builder);
	~SVEModel();

	SVEModel(const SVEModel&) = delete;
	SVEModel& operator=(const SVEModel&) = delete;

	static std::unique_ptr<SVEModel> createModelFromFile(SVEDevice& device, const std::string& filepath);

	void bind(VkCommandBuffer commandBuffer);
	void draw(VkCommandBuffer commandBuffer);

private:
	void createVertexBuffers(const std::vector<Vertex>& vertices);
	void createIndexBuffers(const std::vector<uint32_t>& indices);

	SVEDevice& sveDevice;

	// vertex buffer
	std::unique_ptr<SVEBuffer> vertexBuffer;
	uint32_t vertexCount;

	// index buffer
	bool hasIndexBuffer = false;
	std::unique_ptr<SVEBuffer> indexBuffer;
	uint32_t indexCount;
};

#endif