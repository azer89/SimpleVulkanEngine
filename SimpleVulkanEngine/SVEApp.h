#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEDevice.h"
#include "SVEModel.h"
#include "SVEGameObject.h"
#include "SVEPipeline.h"
#include "SVESwapChain.h"
#include "SVEWindow.h"

// std
#include <memory>
#include <vector>

class SVEApp
{
public:
	static constexpr const char* TITLE = "Simple Vulkan";
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	SVEApp();
	~SVEApp();

	SVEApp(const SVEApp&) = delete;
	SVEApp& operator=(const SVEApp&) = delete;

	void run();
	

private:
	void loadModels();
	void loadGameObjects();
	void createPipelineLayout();
	void createPipeline();
	void createCommandBuffers();
	void freeCommandBuffers();
	void drawFrame();
	void recreateSwapChain();
	void recordCommandBuffer(int imageIndex);
	void renderGameObjects(VkCommandBuffer commandBuffer);

	SVEWindow sveWindow{ WIDTH, HEIGHT, TITLE };
	SVEDevice sveDevice{ sveWindow };
	std::unique_ptr<SVESwapChain> sveSwapChain;
	std::unique_ptr<SVEPipeline> svePipeline;
	VkPipelineLayout pipelineLayout;
	std::vector<VkCommandBuffer> commandBuffers;
	std::unique_ptr<SVEModel> sveModel;
	std::vector<SVEGameObject> gameObjects;
};

#endif