#ifndef SVE_PIPELINE_H
#define SVE_PIPELINE_H

#include "SVEDevice.h"
#include "PipelineConfigInfo.h"

// std
#include <string>
#include <vector>

class SVEPipeline
{
public:
	SVEPipeline(
		SVEDevice& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo);
	~SVEPipeline();

	SVEPipeline(const SVEPipeline&) = delete;
	void operator=(const SVEPipeline&) = delete;

	void bind(VkCommandBuffer commandBuffer);
	static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:
	static std::vector<char> readFile(const std::string& filepath);

	void createGraphicsPipeline(
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo);

	void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

	SVEDevice& sveDevice;
	VkPipeline graphicsPipeline;
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
};

#endif