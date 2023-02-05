#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include "SVECamera.h"

// lib
#include <vulkan/vulkan.h>

struct FrameInfo
{
	int frameIndex;
	float frameTime;
	VkCommandBuffer commandBuffer;
	SVECamera& camera;
};

#endif