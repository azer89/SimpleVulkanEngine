#ifndef SVE_FRAME_BUFFER_H
#define SVE_FRAME_BUFFER_H

#include "SVEDevice.h"
#include "SVERenderPass.h"

class SVEFrameBuffer
{
public:
	SVEFrameBuffer(
		const std::shared_ptr<SVEDevice>& device,
		const std::shared_ptr<SVERenderPass>& renderPass,
		VkImageView swapChainImageView,
		VkImageView depthImageView,
		uint32_t width,
		uint32_t height
	);
	SVEFrameBuffer();
	~SVEFrameBuffer();

	//SVEFrameBuffer(const SVEFrameBuffer&) = default;
	//SVEFrameBuffer& operator=(const SVEFrameBuffer&) = default;

	VkFramebuffer getFrameBuffer() const { return frameBuffer; }

private:
	std::shared_ptr<SVEDevice> sveDevice;
	VkFramebuffer frameBuffer = VK_NULL_HANDLE;
};

#endif