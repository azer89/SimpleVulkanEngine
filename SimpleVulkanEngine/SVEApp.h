#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEWindow.h"
#include "SVEPipeline.h"
#include "SVEEngine.h"
#include "SVESwapChain.h"

#include <memory>
#include <vector>

class SVEApp
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;
	static constexpr const char* TITLE = "Simple Vulkan";
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";

	SVEApp();
	~SVEApp();

	SVEApp(const SVEApp&) = delete;
	SVEApp& operator=(const SVEApp&) = delete;

	void Run();

private:
	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
	void drawFrame();

	//SVEPipeline svePipeline;
	SVEWindow sveWindow{WIDTH, HEIGHT, TITLE};
	SVEEngine sveEngine{sveWindow};
	SVESwapChain sveSwapChain{sveEngine, sveWindow.getExtent()};
	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	//SVEPipeline svePipeline{sveEngine, VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH, SVEPipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT) };
};

#endif