#ifndef SVE_TEXTURE_H
#define SVE_TEXTURE_H

#include "SVEDevice.h"

// vulkan headers
#include <vulkan/vulkan.h>

class SVETexture
{
	SVETexture(SVEDevice& device);
	~SVETexture();

private:
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
	SVEDevice& sveDevice;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

#endif
