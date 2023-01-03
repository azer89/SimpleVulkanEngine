#ifndef SVE_PIPELINE_H
#define SVE_PIPELINE_H

#include "SVEEngine.h"

// std
#include <string>
#include <vector>


struct PipelineConfigInfo
{
	PipelineConfigInfo() = default;
	PipelineConfigInfo(const PipelineConfigInfo&) = delete;
	PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo viewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
	std::vector<VkDynamicState> dynamicStateEnables;
	VkPipelineDynamicStateCreateInfo dynamicStateInfo;
	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class SVEPipeline
{
public:
	SVEPipeline(
		SVEEngine& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo);
	~SVEPipeline();

	SVEPipeline(const SVEPipeline&) = delete;
	void operator=(const SVEPipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer);

	static void defaultPipelineConfigInfo(
		PipelineConfigInfo& configInfo);

private:
	static std::vector<char> readFile(const std::string& filepath);

	void createGraphicsPipeline(
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo);

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	SVEEngine& sveDevice;
	VkPipeline graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};

#endif