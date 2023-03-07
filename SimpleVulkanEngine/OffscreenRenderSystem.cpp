#include "OffscreenRenderSystem.h"

#include <array>
#include <stdexcept>

#define SHADOWMAP_DIM 2048
#define DEFAULT_SHADOWMAP_FILTER VK_FILTER_LINEAR
#define DEPTH_FORMAT VK_FORMAT_D16_UNORM

OffscreenRenderSystem::OffscreenRenderSystem(const std::shared_ptr<SVEDevice>& device) : sveDevice(device)
{
}

OffscreenRenderSystem::~OffscreenRenderSystem()
{
}

// Set up a separate render pass for the offscreen frame buffer
// This is necessary as the offscreen frame buffer attachments use formats different to those from the example render pass
void OffscreenRenderSystem::prepareOffscreenRenderpass()
{
	VkAttachmentDescription attachmentDescription{};
	attachmentDescription.format = DEPTH_FORMAT;
	attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
	attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;							// Clear depth at beginning of the render pass
	attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;						// We will read from depth, so it's important to store the depth attachment results
	attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;					// We don't care about initial layout of the attachment
	attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;// Attachment will be transitioned to shader read at render pass end

	VkAttachmentReference depthReference = {};
	depthReference.attachment = 0;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;			// Attachment will be used as depth/stencil during render pass

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 0;													// No color attachments
	subpass.pDepthStencilAttachment = &depthReference;									// Reference to our depth attachment

	// Use subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassCreateInfo{};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &attachmentDescription;
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassCreateInfo.pDependencies = dependencies.data();

	if (vkCreateRenderPass(sveDevice->device(), &renderPassCreateInfo, nullptr, &offscreenPass.renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an offscreen render pass");
	}
}

// Setup the offscreen framebuffer for rendering the scene from light's point-of-view to
// The depth attachment of this framebuffer will then be used to sample from in the fragment shader of the shadowing pass
void OffscreenRenderSystem::prepareOffscreenFramebuffer()
{
	offscreenPass.width = SHADOWMAP_DIM;
	offscreenPass.height = SHADOWMAP_DIM;

	// For shadow mapping we only need a depth attachment
	VkImageCreateInfo image{};
	image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.extent.width = offscreenPass.width;
	image.extent.height = offscreenPass.height;
	image.extent.depth = 1;
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.format = DEPTH_FORMAT;																// Depth stencil attachment
	image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;		// We will sample directly from the depth attachment for the shadow mapping
	if (vkCreateImage(sveDevice->device(), &image, nullptr, &offscreenPass.depth.image) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an offscreen depth image");
	}

	VkMemoryAllocateInfo memAlloc{};
	memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	VkMemoryRequirements memReqs;
	vkGetImageMemoryRequirements(sveDevice->device(), offscreenPass.depth.image, &memReqs);
	memAlloc.allocationSize = memReqs.size;
	memAlloc.memoryTypeIndex = sveDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	if (vkAllocateMemory(sveDevice->device(), &memAlloc, nullptr, &offscreenPass.depth.mem) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate memory for offscreen depth image");
	}
	if (vkBindImageMemory(sveDevice->device(), offscreenPass.depth.image, offscreenPass.depth.mem, 0) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to bind memory for offscreen depth image");
	}

	VkImageViewCreateInfo depthStencilView;
	depthStencilView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
	depthStencilView.format = DEPTH_FORMAT;
	depthStencilView.subresourceRange = {};
	depthStencilView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	depthStencilView.subresourceRange.baseMipLevel = 0;
	depthStencilView.subresourceRange.levelCount = 1;
	depthStencilView.subresourceRange.baseArrayLayer = 0;
	depthStencilView.subresourceRange.layerCount = 1;
	depthStencilView.image = offscreenPass.depth.image;
	if (vkCreateImageView(sveDevice->device(), &depthStencilView, nullptr, &offscreenPass.depth.view) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an offscreen depth image view");
	}

	// Create sampler to sample from to depth attachment
	// Used to sample in the fragment shader for shadowed rendering
	VkFilter shadowmap_filter = sveDevice->formatIsFilterable(DEPTH_FORMAT, VK_IMAGE_TILING_OPTIMAL) ?
		DEFAULT_SHADOWMAP_FILTER :
		VK_FILTER_NEAREST;
	VkSamplerCreateInfo sampler{};
	sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler.maxAnisotropy = 1.0f;
	sampler.magFilter = shadowmap_filter;
	sampler.minFilter = shadowmap_filter;
	sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler.addressModeV = sampler.addressModeU;
	sampler.addressModeW = sampler.addressModeU;
	sampler.mipLodBias = 0.0f;
	sampler.maxAnisotropy = 1.0f;
	sampler.minLod = 0.0f;
	sampler.maxLod = 1.0f;
	sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	if (vkCreateSampler(sveDevice->device(), &sampler, nullptr, &offscreenPass.depthSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an offscreen depth image sampler");
	}

	prepareOffscreenRenderpass();

	// Create frame buffer
	VkFramebufferCreateInfo fbufCreateInfo{};
	fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	fbufCreateInfo.renderPass = offscreenPass.renderPass;
	fbufCreateInfo.attachmentCount = 1;
	fbufCreateInfo.pAttachments = &offscreenPass.depth.view;
	fbufCreateInfo.width = offscreenPass.width;
	fbufCreateInfo.height = offscreenPass.height;
	fbufCreateInfo.layers = 1;

	if (vkCreateFramebuffer(sveDevice->device(), &fbufCreateInfo, nullptr, &offscreenPass.frameBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an offscreen frame buffer");
	}
}

void OffscreenRenderSystem::buildCommandBuffers()
{
	VkCommandBufferBeginInfo cmdBufInfo{};
	cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkClearValue clearValues[2];
	VkViewport viewport;
	VkRect2D scissor;

	// TODO
}