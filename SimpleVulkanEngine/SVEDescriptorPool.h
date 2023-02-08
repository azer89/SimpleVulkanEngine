#ifndef SVE_DESCRIPTOR_POOL_H
#define SVE_DESCRIPTOR_POOL_H

#include "SVEDevice.h"

#include <memory>

class SVEDescriptorPool
{
public:
	class Builder
	{
	public:
		Builder(SVEDevice& sveDevice) : sveDevice{ sveDevice } {}

		Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& setMaxSets(uint32_t count);
		std::unique_ptr<SVEDescriptorPool> build() const;

	private:
		SVEDevice& sveDevice;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 1000;
		VkDescriptorPoolCreateFlags poolFlags = 0;
	};

	SVEDescriptorPool(
		SVEDevice& sveDevice,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes);
	~SVEDescriptorPool();
	SVEDescriptorPool(const SVEDescriptorPool&) = delete;
	SVEDescriptorPool& operator=(const SVEDescriptorPool&) = delete;

	bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

	void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

	void resetPool();

private:
	SVEDevice& sveDevice;
	VkDescriptorPool descriptorPool;

	friend class SVEDescriptorWriter;
};

#endif