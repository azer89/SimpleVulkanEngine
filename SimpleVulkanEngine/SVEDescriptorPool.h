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
		Builder(const std::shared_ptr<SVEDevice>& device) : sveDevice{ device } {}

		Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
		Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
		Builder& setMaxSets(uint32_t count);
		std::unique_ptr<SVEDescriptorPool> build() const;

	private:
		std::shared_ptr<SVEDevice> sveDevice;
		std::vector<VkDescriptorPoolSize> poolSizes{};
		uint32_t maxSets = 1000;
		VkDescriptorPoolCreateFlags poolFlags = 0;
	};

	SVEDescriptorPool(
		const std::shared_ptr<SVEDevice>& device,
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
	std::shared_ptr<SVEDevice> sveDevice;
	VkDescriptorPool descriptorPool;

	friend class SVEDescriptorWriter;
};

#endif