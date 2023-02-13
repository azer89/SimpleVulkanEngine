#ifndef FRAME_INFO_H
#define FRAME_INFO_H

#include "SVECamera.h"
#include "SVEGameObject.h"

// lib
#include <vulkan/vulkan.h>

struct FrameInfo
{
	int frameIndex;
	float deltaTime;
	VkCommandBuffer commandBuffer;
	SVECamera& camera;
	VkDescriptorSet globalDescriptorSet;
	SVEGameObject::Map& gameObjects;
};

#endif