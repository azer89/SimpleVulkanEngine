#ifndef CIRCLE_BILLBOARD_RENDER_SYSTEM_H
#define CIRCLE_BILLBOARD_RENDER_SYSTEM_H

#include "SVECamera.h"
#include "SVEDevice.h"
#include "FrameInfo.h"
#include "GlobalUBO.h"
#include "SVEGameObject.h"
#include "SVEPipeline.h"

// std
#include <memory>
#include <vector>

class CircleBillboardRenderSystem
{
private:
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/circle_billboard_vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/circle_billboard_frag.spv";

public:
	CircleBillboardRenderSystem(SVEDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~CircleBillboardRenderSystem();

	CircleBillboardRenderSystem(const CircleBillboardRenderSystem&) = delete;
	CircleBillboardRenderSystem& operator=(const CircleBillboardRenderSystem&) = delete;

	void update(FrameInfo& frameInfo, GlobalUbo& ubo);
	void render(const FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	SVEDevice& sveDevice;

	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
};

#endif