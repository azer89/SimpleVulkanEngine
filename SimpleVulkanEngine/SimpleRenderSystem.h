#ifndef SIMPLE_RENDER_SYSTEM_H
#define SIMPLE_RENDER_SYSTEM_H

#include "SVEDevice.h"
#include "SVEGameObject.h"
#include "SVEPipeline.h"
#include "SVECamera.h"
#include "FrameInfo.h"

// std
#include <memory>
#include <vector>

class SimpleRenderSystem
{
private:
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/simple_shader_vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/simple_shader_frag.spv";

public:
	SimpleRenderSystem(const std::shared_ptr<SVEDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
	~SimpleRenderSystem();

	SimpleRenderSystem(const SimpleRenderSystem&) = delete;
	SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

	void render(const FrameInfo& frameInfo);

private:
	void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	void createPipeline(VkRenderPass renderPass);

	std::shared_ptr<SVEDevice> sveDevice;
	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
};

#endif
