#ifndef SVE_SWAP_CHAIN_H
#define SVE_SWAP_CHAIN_H

#include "SVEDevice.h"
#include "SVERenderPass.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

class SVESwapChain
{
public:
	static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

	SVESwapChain(const std::shared_ptr<SVEDevice>& deviceRef, VkExtent2D windowExtent);
	SVESwapChain(const std::shared_ptr<SVEDevice>& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SVESwapChain> previous);
	~SVESwapChain();

	SVESwapChain(const SVESwapChain&) = delete;
	SVEDevice& operator=(const SVESwapChain&) = delete;

	// TODO change the return type
	VkFramebuffer getFrameBuffer(int index) const { return swapChainFramebuffers[index]; }
	VkRenderPass getRenderPass() const { return renderPass.getRenderPass(); }

	VkImageView getImageView(int index) const { return swapChainImageViews[index]; }
	size_t imageCount() const { return swapChainImages.size(); }
	VkFormat getSwapChainImageFormat() const { return swapChainImageFormat; }
	VkExtent2D getSwapChainExtent() const { return swapChainExtent; }
	uint32_t width() const { return swapChainExtent.width; }
	uint32_t height() const { return swapChainExtent.height; }
	float extentAspectRatio() const { return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height); }

	VkResult acquireNextImage(uint32_t* imageIndex);
	VkResult submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex);

	bool compareSwapFormats(const SVESwapChain& swapChain) const
	{
		return swapChain.swapChainDepthFormat == swapChainDepthFormat &&
			swapChain.swapChainImageFormat == swapChainImageFormat;
	}

private:
	void init();
	void createSwapChain();
	void createImageViews();
	void createDepthResources();
	void createFramebuffers();
	void createSyncObjects();

	// Helper functions
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	VkFormat swapChainImageFormat;
	VkFormat swapChainDepthFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkFramebuffer> swapChainFramebuffers;
	SVERenderPass renderPass;

	std::vector<VkImage> depthImages;
	std::vector<VkDeviceMemory> depthImageMemories;
	std::vector<VkImageView> depthImageViews;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	std::shared_ptr<SVEDevice> device;
	VkExtent2D windowExtent;

	VkSwapchainKHR swapChain;
	std::shared_ptr<SVESwapChain> oldSwapChain;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;
};

#endif