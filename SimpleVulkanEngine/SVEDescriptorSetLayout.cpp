#include "SVEDescriptorSetLayout.h"

#include <cassert>
#include <stdexcept>

// *************** Descriptor Set Layout Builder *********************

SVEDescriptorSetLayout::Builder& SVEDescriptorSetLayout::Builder::addBinding(
	uint32_t binding,
	VkDescriptorType descriptorType,
	VkShaderStageFlags stageFlags,
	uint32_t count)
{
	assert(bindings.count(binding) == 0 && "Binding already in use");
	VkDescriptorSetLayoutBinding layoutBinding{};
	layoutBinding.binding = binding;
	layoutBinding.descriptorType = descriptorType;
	layoutBinding.descriptorCount = count;
	layoutBinding.stageFlags = stageFlags;
	bindings[binding] = layoutBinding;
	return *this;
}

std::unique_ptr<SVEDescriptorSetLayout> SVEDescriptorSetLayout::Builder::build() const
{
	return std::make_unique<SVEDescriptorSetLayout>(sveDevice, bindings);
}

// *************** Descriptor Set Layout *********************

SVEDescriptorSetLayout::SVEDescriptorSetLayout(
	const std::shared_ptr<SVEDevice>& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings)
	: sveDevice{ device }, bindings{ bindings } {
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
	for (auto& kv : bindings)
	{
		setLayoutBindings.push_back(kv.second);
	}

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
	descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
	descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

	if (vkCreateDescriptorSetLayout(
		sveDevice->device(),
		&descriptorSetLayoutInfo,
		nullptr,
		&descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

SVEDescriptorSetLayout::~SVEDescriptorSetLayout()
{
	vkDestroyDescriptorSetLayout(sveDevice->device(), descriptorSetLayout, nullptr);
}