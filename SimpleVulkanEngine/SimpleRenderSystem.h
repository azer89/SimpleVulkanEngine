#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "SVEDevice.h"
#include "SVEGameObject.h"
#include "SVEPipeline.h"

// std
#include <memory>
#include <vector>

class SimpleRenderSystem
{
public:
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";

public:
	SimpleRenderSystem(SVEDevice& device, VkRenderPass renderPass);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SVEGameObject>& gameObjects);

private:
	void createPipelineLayout();
	void createPipeline(VkRenderPass renderPass);

	SVEDevice& sveDevice;

	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
};

#endif
