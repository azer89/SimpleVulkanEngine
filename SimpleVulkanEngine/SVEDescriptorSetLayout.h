#ifndef SVE_DESCRIPTOR_SET_LAYOUT_H
#define SVE_DESCRIPTOR_SET_LAYOUT_H

#include "SVEDevice.h"

#include <memory>
#include <unordered_map>

class SVEDescriptorSetLayout
{
public:
	class Builder
	{
	public:
		Builder(SVEDevice& sveDevice) : sveDevice{ sveDevice } {}

		Builder& addBinding(
			uint32_t binding,
			VkDescriptorType descriptorType,
			VkShaderStageFlags stageFlags,
			uint32_t count = 1);
		std::unique_ptr<SVEDescriptorSetLayout> build() const;

	private:
		SVEDevice& sveDevice;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
	};

	SVEDescriptorSetLayout(SVEDevice& sveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
	~SVEDescriptorSetLayout();

	// Non copyable
	SVEDescriptorSetLayout(const SVEDescriptorSetLayout&) = delete;
	SVEDescriptorSetLayout& operator=(const SVEDescriptorSetLayout&) = delete;

	VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

private:
	SVEDevice& sveDevice;
	VkDescriptorSetLayout descriptorSetLayout;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

	friend class SVEDescriptorWriter;
};

#endif