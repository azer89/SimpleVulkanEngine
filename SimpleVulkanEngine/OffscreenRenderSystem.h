#ifndef OFFSCREEN_RENDER_SYSTEM_H
#define OFFSCREEN_RENDER_SYSTEM_H

#include "SVECamera.h"
#include "SVEDevice.h"
#include "FrameInfo.h"
#include "GlobalUBO.h"
#include "SVEGameObject.h"
#include "SVEPipeline.h"

// std
#include <memory>
#include <vector>

class OfflineRenderSystem
{
private:
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/_.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/_.spv";

public:
	OfflineRenderSystem(std::shared_ptr<SVEDevice> device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~OfflineRenderSystem();

	OfflineRenderSystem(const OfflineRenderSystem&) = delete;
	OfflineRenderSystem& operator=(const OfflineRenderSystem&) = delete;

	void render(const FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	std::shared_ptr<SVEDevice> sveDevice;

	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
};

#endif
