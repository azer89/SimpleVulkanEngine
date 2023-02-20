#include "SVEFrameBuffer.h"

SVEFrameBuffer::SVEFrameBuffer(
	const std::shared_ptr<SVEDevice>& device,
	const std::shared_ptr<SVERenderPass>& renderPass,
	VkImageView swapChainImageView,
	VkImageView depthImageView,
	uint32_t width,
	uint32_t height
)
{
	sveDevice = device;
	frameBuffer = sveDevice->createFrameBuffer(
		renderPass->getRenderPass(),
		depthImageView,
		depthImageView,
		width,
		height);
}

SVEFrameBuffer::SVEFrameBuffer()
{
}

SVEFrameBuffer::~SVEFrameBuffer()
{
	if (frameBuffer != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(sveDevice->device(), frameBuffer, nullptr);
	}
}

