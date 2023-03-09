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

struct FrameBufferAttachment
{
	VkImage image;
	VkDeviceMemory mem;
	VkImageView view;
};

class OffscreenRenderSystem
{
private:
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/_.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/_.spv";

public:
	OffscreenRenderSystem(const std::shared_ptr<SVEDevice>& device);
	~OffscreenRenderSystem();

	OffscreenRenderSystem(const OffscreenRenderSystem&) = delete;
	OffscreenRenderSystem& operator=(const OffscreenRenderSystem&) = delete;

	//void render(const FrameInfo& frameInfo);

private:
	void prepareOffscreenRenderpass();
	void prepareOffscreenFramebuffer();
	void setupDescriptorSetLayout();
	void setupDescriptorPool();
	void setupDescriptorSets();
	void buildCommandBuffers();
	//void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
	//void createPipeline(VkRenderPass renderPass);

	std::shared_ptr<SVEDevice> sveDevice;

	//std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
	VkDescriptorSetLayout descriptorSetLayout;

	struct OffscreenPass
	{
		int32_t width;
		int32_t height;
		VkFramebuffer frameBuffer;
		FrameBufferAttachment depth;
		VkRenderPass renderPass;
		VkSampler depthSampler;
		VkDescriptorImageInfo descriptor;
	} offscreenPass;
};

#endif
