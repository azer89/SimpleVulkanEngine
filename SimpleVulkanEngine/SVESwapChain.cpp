#include "SVESwapChain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>
#include <memory>


SVESwapChain::SVESwapChain(SVEDevice& deviceRef, VkExtent2D extent)
	: device(deviceRef), renderPass{deviceRef}, windowExtent{extent}
{
	init();
}

SVESwapChain::SVESwapChain(
	SVEDevice& deviceRef, VkExtent2D extent, std::shared_ptr<SVESwapChain> previous)
	: device{ deviceRef }, renderPass{ deviceRef }, windowExtent{ extent }, oldSwapChain{ previous }
{
	init();

	// Not destroying the old swap chain as it will be provided to VkSwapchainCreateInfoKHR() when recerating a new one
	//oldSwapChain = nullptr; // delete the old swap chain
}

SVESwapChain::~SVESwapChain()
{
	for (auto imageView : swapChainImageViews)
	{
		vkDestroyImageView(device.device(), imageView, nullptr);
	}
	swapChainImageViews.clear();

	if (swapChain != nullptr)
	{
		vkDestroySwapchainKHR(device.device(), swapChain, nullptr);
		swapChain = nullptr;
	}

	for (int i = 0; i < depthImages.size(); ++i)
	{
		vkDestroyImageView(device.device(), depthImageViews[i], nullptr);
		vkDestroyImage(device.device(), depthImages[i], nullptr);
		vkFreeMemory(device.device(), depthImageMemories[i], nullptr);
	}

	for (auto& framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(device.device(), framebuffer, nullptr);
	}

	// cleanup synchronization objects
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroySemaphore(device.device(), renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device.device(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device.device(), inFlightFences[i], nullptr);
	}
}

void SVESwapChain::init()
{
	createSwapChain();
	createImageViews();
	createDepthResources();
	createFramebuffers();
	createSyncObjects();
}

VkResult SVESwapChain::acquireNextImage(uint32_t* imageIndex)
{
	vkWaitForFences(
		device.device(),
		1,
		&inFlightFences[currentFrame],
		VK_TRUE,
		std::numeric_limits<uint64_t>::max());

	VkResult result = vkAcquireNextImageKHR(
		device.device(),
		swapChain,
		std::numeric_limits<uint64_t>::max(),
		imageAvailableSemaphores[currentFrame],  // must be a not signaled semaphore
		VK_NULL_HANDLE,
		imageIndex);

	return result;
}

VkResult SVESwapChain::submitCommandBuffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
{
	if (imagesInFlight[*imageIndex] != VK_NULL_HANDLE)
	{
		vkWaitForFences(device.device(), 1, &imagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[*imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = buffers;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device.device(), 1, &inFlightFences[currentFrame]);
	if (vkQueueSubmit(device.graphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = imageIndex;

	auto result = vkQueuePresentKHR(device.presentQueue(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	return result;
}

void SVESwapChain::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = device.getSwapChainSupport();
	VkSurfaceFormatKHR surfaceFormat = device.chooseSwapSurfaceFormat(swapChainSupport.formats);

	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = device.surface();

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = device.findPhysicalQueueFamilies();
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily, indices.presentFamily };

	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;      // Optional
		createInfo.pQueueFamilyIndices = nullptr;  // Optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = oldSwapChain == nullptr ? VK_NULL_HANDLE : oldSwapChain->swapChain;

	if (vkCreateSwapchainKHR(device.device(), &createInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create swap chain!");
	}

	// we only specified a minimum number of images in the swap chain, so the implementation is
	// allowed to create a swap chain with more. That's why we'll first query the final number of
	// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
	// retrieve the handles.
	vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device.device(), swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
}

void SVESwapChain::createImageViews()
{
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		// TODO can be moved to function createImageView
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = swapChainImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = swapChainImageFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.device(), &viewInfo, nullptr, &swapChainImageViews[i]) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view!");
		}
	}
}

void SVESwapChain::createFramebuffers()
{
	VkExtent2D swapChainExtent = getSwapChainExtent();
	swapChainFramebuffers.resize(imageCount());
	for (size_t i = 0; i < imageCount(); ++i)
	{
		swapChainFramebuffers[i] = device.createFrameBuffer(
			renderPass.getRenderPass(),
			swapChainImageViews[i], 
			depthImageViews[i], 
			swapChainExtent.width, 
			swapChainExtent.height);
	}
}

void SVESwapChain::createDepthResources()
{
	VkFormat depthFormat = device.getDepthFormat();
	swapChainDepthFormat = depthFormat;
	VkExtent2D swapChainExtent = getSwapChainExtent();

	depthImages.resize(imageCount());
	depthImageMemories.resize(imageCount());
	depthImageViews.resize(imageCount());

	for (int i = 0; i < depthImages.size(); ++i)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = swapChainExtent.width;
		imageInfo.extent.height = swapChainExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = depthFormat;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = 0;

		device.createImageWithInfo(
			imageInfo,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			depthImages[i],
			depthImageMemories[i]);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = depthImages[i];
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = depthFormat;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device.device(), &viewInfo, nullptr, &depthImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create texture image view!");
		}
	}
}

void SVESwapChain::createSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device.device(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device.device(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create synchronization objects for a frame!");
		}
	}
}

VkPresentModeKHR SVESwapChain::chooseSwapPresentMode(
	const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			std::cout << "Present mode: Mailbox" << std::endl;
			return availablePresentMode;
		}
	}

	// for (const auto &availablePresentMode : availablePresentModes) {
	//   if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
	//     std::cout << "Present mode: Immediate" << std::endl;
	//     return availablePresentMode;
	//   }
	// }

	std::cout << "Present mode: V-Sync" << std::endl;
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SVESwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = windowExtent;
		actualExtent.width = std::max(
			capabilities.minImageExtent.width,
			std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
			capabilities.minImageExtent.height,
			std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}
}
