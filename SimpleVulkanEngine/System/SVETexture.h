#ifndef SVE_TEXTURE_H
#define SVE_TEXTURE_H

#include "SVEDevice.h"

#include <memory>

class SVETexture
{
public:
	SVETexture(const std::shared_ptr<SVEDevice>& device, const char* path);
	~SVETexture();

	VkDescriptorImageInfo descriptorImageInfo();

private:
	void createTextureImage();
	void createTextureImageView();
	void createTextureSampler();

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

private:
	const char* imagePath;

	std::shared_ptr<SVEDevice> sveDevice;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

#endif
