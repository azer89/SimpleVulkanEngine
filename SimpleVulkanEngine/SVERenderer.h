#ifndef SVE_RENDERER_H
#define SVE_RENDERER_H

#include "SVEDevice.h"
#include "SVESwapChain.h"
#include "SVEWindow.h"

// std
#include <cassert>
#include <memory>
#include <vector>

class SVERenderer
{
public:
	SVERenderer(SVEWindow& window, SVEDevice& device);
	~SVERenderer();

	SVERenderer(const SVERenderer&) = delete;
	SVERenderer& operator=(const SVERenderer&) = delete;

	VkRenderPass getSwapChainRenderPass() const { return sveSwapChain->getRenderPass(); }
	bool isFrameInProgress() const { return isFrameStarted; }

	VkCommandBuffer getCurrentCommandBuffer() const
	{
		assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
		return commandBuffers[currentFrameIndex];
	}

	int getFrameIndex() const
	{
		assert(isFrameStarted && "Cannot get frame index when frame not in progress");
		return currentFrameIndex;
	}

	VkCommandBuffer beginFrame();
	void endFrame();
	void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
	void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

private:
	void createCommandBuffers();
	void freeCommandBuffers();
	void recreateSwapChain();

	SVEWindow& sveWindow;
	SVEDevice& sveDevice;
	std::unique_ptr<SVESwapChain> sveSwapChain;
	std::vector<VkCommandBuffer> commandBuffers;

	uint32_t currentImageIndex;
	int currentFrameIndex = 0;
	bool isFrameStarted = false; // should be instantiated false
};

#endif