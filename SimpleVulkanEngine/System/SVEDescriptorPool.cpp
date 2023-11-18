#include "SVEDescriptorPool.h"
#include <stdexcept>

// *************** Descriptor Pool Builder *********************

SVEDescriptorPool::Builder& SVEDescriptorPool::Builder::addPoolSize(VkDescriptorType descriptorType, uint32_t count)
{
	poolSizes.push_back({ descriptorType, count });
	return *this;
}

SVEDescriptorPool::Builder& SVEDescriptorPool::Builder::setPoolFlags(
	VkDescriptorPoolCreateFlags flags)
{
	poolFlags = flags;
	return *this;
}

SVEDescriptorPool::Builder& SVEDescriptorPool::Builder::setMaxSets(uint32_t count)
{
	maxSets = count;
	return *this;
}

std::unique_ptr<SVEDescriptorPool> SVEDescriptorPool::Builder::build() const
{
	return std::make_unique<SVEDescriptorPool>(sveDevice, maxSets, poolFlags, poolSizes);
}

// *************** Descriptor Pool *********************

SVEDescriptorPool::SVEDescriptorPool(
	const std::shared_ptr<SVEDevice>& device,
	uint32_t maxSets,
	VkDescriptorPoolCreateFlags poolFlags,
	const std::vector<VkDescriptorPoolSize>& poolSizes)
	: sveDevice{ device }
{
	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	descriptorPoolInfo.pPoolSizes = poolSizes.data();
	descriptorPoolInfo.maxSets = maxSets;
	descriptorPoolInfo.flags = poolFlags;

	if (vkCreateDescriptorPool(sveDevice->device(), &descriptorPoolInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

SVEDescriptorPool::~SVEDescriptorPool()
{
	vkDestroyDescriptorPool(sveDevice->device(), descriptorPool, nullptr);
}

bool SVEDescriptorPool::allocateDescriptor(
	const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
{
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.pSetLayouts = &descriptorSetLayout;
	allocInfo.descriptorSetCount = 1;

	// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
	// a new pool whenever an old pool fills up. But this is beyond our current scope
	if (vkAllocateDescriptorSets(sveDevice->device(), &allocInfo, &descriptor) != VK_SUCCESS)
	{
		return false;
	}
	return true;
}

void SVEDescriptorPool::freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const
{
	vkFreeDescriptorSets(
		sveDevice->device(),
		descriptorPool,
		static_cast<uint32_t>(descriptors.size()),
		descriptors.data());
}

void SVEDescriptorPool::resetPool()
{
	vkResetDescriptorPool(sveDevice->device(), descriptorPool, 0);
}
