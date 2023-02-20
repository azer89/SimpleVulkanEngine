#include "CircleBillboardRenderSystem.h"
#include "GlobalUBO.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

CircleBillboardRenderSystem::CircleBillboardRenderSystem(
	const std::shared_ptr<SVEDevice>& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
	: sveDevice{ device }
{
	createPipelineLayout(globalSetLayout);
	createPipeline(renderPass);
}

CircleBillboardRenderSystem::~CircleBillboardRenderSystem()
{
	vkDestroyPipelineLayout(sveDevice->device(), pipelineLayout, nullptr);
}

void CircleBillboardRenderSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout)
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(CircleBillboardPushConstants);

	std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(sveDevice->device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout");
	}
}

void CircleBillboardRenderSystem::createPipeline(VkRenderPass renderPass)
{
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	SVEPipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.attributeDescriptions.clear();
	pipelineConfig.bindingDescriptions.clear();
	pipelineConfig.renderPass = renderPass;
	pipelineConfig.pipelineLayout = pipelineLayout;
	pipelineConfig.constantInfo = { MAX_LIGHTS, MAX_OBJECTS };
	svePipeline = std::make_unique<SVEPipeline>(
		sveDevice,
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH,
		pipelineConfig);
}

void CircleBillboardRenderSystem::render(const FrameInfo& frameInfo)
{
	svePipeline->bind(frameInfo.commandBuffer);

	vkCmdBindDescriptorSets(
		frameInfo.commandBuffer,
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		pipelineLayout,
		0,
		1,
		&frameInfo.globalDescriptorSet,
		0,
		nullptr);

	for (auto& kv : frameInfo.gameObjects)
	{
		auto& obj = kv.second;
		if (obj.pointLight == nullptr)
		{
			continue;
		}

		CircleBillboardPushConstants push{};
		push.position = glm::vec4(obj.transform.translation, 1.f);
		push.color = glm::vec4(obj.color, obj.pointLight->lightIntensity);
		push.radius = obj.transform.scale.x;

		vkCmdPushConstants(
			frameInfo.commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(CircleBillboardPushConstants),
			&push);

		// TODO need to create a separate frameInfo
		vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
	}
}