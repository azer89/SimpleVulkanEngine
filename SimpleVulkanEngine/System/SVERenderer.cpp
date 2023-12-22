#include "SVERenderer.h"

// std
#include <array>
#include <cassert>
#include <stdexcept>

SVERenderer::SVERenderer(const std::shared_ptr<SVEWindow>& window, const std::shared_ptr<SVEDevice>& device)
	: sveWindow{ window }, sveDevice{ device } 
{
	recreateSwapChain();
	createCommandBuffers();
}

SVERenderer::~SVERenderer() 
{ 
	freeCommandBuffers();
}

// Recreate a swapchain if the window is resized
void SVERenderer::recreateSwapChain()
{
	auto extent = sveWindow->getExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = sveWindow->getExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(sveDevice->device());

	if (sveSwapChain == nullptr)
	{
		sveSwapChain = std::make_unique<SVESwapChain>(sveDevice, extent);
	}
	else
	{
		std::shared_ptr<SVESwapChain> oldSwapChain = std::move(sveSwapChain);
		sveSwapChain = std::make_unique<SVESwapChain>(sveDevice, extent, oldSwapChain);
		 
		if (!oldSwapChain->compareSwapFormats(*sveSwapChain.get()))
		{
			throw std::runtime_error("Swap chain image(or depth) format has changed");
		}
	}
}

void SVERenderer::createCommandBuffers()
{
	commandBuffers.resize(SVESwapChain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = sveDevice->getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(sveDevice->device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers");
	}
}

void SVERenderer::freeCommandBuffers()
{
	vkFreeCommandBuffers(
		sveDevice->device(),
		sveDevice->getCommandPool(),
		static_cast<uint32_t>(commandBuffers.size()),
		commandBuffers.data());
	commandBuffers.clear();
}

VkCommandBuffer SVERenderer::beginFrame()
{
	assert(!isFrameStarted && "Can't call beginFrame while already in progress");

	auto result = sveSwapChain->acquireNextImage(&currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return nullptr;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image");
	}

	isFrameStarted = true;

	auto commandBuffer = getCurrentCommandBuffer();
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer");
	}
	return commandBuffer;
}

void SVERenderer::endFrame()
{
	assert(isFrameStarted && "Can't call endFrame while frame is not in progress");
	auto commandBuffer = getCurrentCommandBuffer();
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer");
	}

	auto result = sveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || sveWindow->wasWindowResized())
	{
		sveWindow->resetWindowResizedFlag();
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}

	isFrameStarted = false;
	currentFrameIndex = (currentFrameIndex + 1) % SVESwapChain::MAX_FRAMES_IN_FLIGHT;
}

void SVERenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
	assert( commandBuffer == getCurrentCommandBuffer() &&
		"Can't begin render pass on command buffer from a different frame");

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = sveSwapChain->getRenderPass();
	renderPassInfo.framebuffer = sveSwapChain->getFrameBuffer(currentImageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = sveSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Viewport
	float w = static_cast<float>(sveSwapChain->getSwapChainExtent().width);
	float h = static_cast<float>(sveSwapChain->getSwapChainExtent().height);
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = w;
	viewport.height = h;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, sveSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void SVERenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
	assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
	assert(
		commandBuffer == getCurrentCommandBuffer() &&
		"Can't end render pass on command buffer from a different frame");
	vkCmdEndRenderPass(commandBuffer);
}