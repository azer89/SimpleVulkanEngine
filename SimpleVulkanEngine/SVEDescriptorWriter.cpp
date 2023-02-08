#include "SVEDescriptorWriter.h"
#include <cassert>

// *************** Descriptor Writer *********************

SVEDescriptorWriter::SVEDescriptorWriter(SVEDescriptorSetLayout& setLayout, SVEDescriptorPool& pool)
	: setLayout{ setLayout }, pool{ pool } {}

SVEDescriptorWriter& SVEDescriptorWriter::writeBuffer(
	uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(
		bindingDescription.descriptorCount == 1 &&
		"Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pBufferInfo = bufferInfo;
	write.descriptorCount = 1;

	writes.push_back(write);
	return *this;
}

SVEDescriptorWriter& SVEDescriptorWriter::writeImage(
	uint32_t binding, VkDescriptorImageInfo* imageInfo)
{
	assert(setLayout.bindings.count(binding) == 1 && "Layout does not contain specified binding");

	auto& bindingDescription = setLayout.bindings[binding];

	assert(
		bindingDescription.descriptorCount == 1 &&
		"Binding single descriptor info, but binding expects multiple");

	VkWriteDescriptorSet write{};
	write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType = bindingDescription.descriptorType;
	write.dstBinding = binding;
	write.pImageInfo = imageInfo;
	write.descriptorCount = 1;

	writes.push_back(write);
	return *this;
}

bool SVEDescriptorWriter::build(VkDescriptorSet& set)
{
	bool success = pool.allocateDescriptor(setLayout.getDescriptorSetLayout(), set);
	if (!success)
	{
		return false;
	}
	overwrite(set);
	return true;
}

void SVEDescriptorWriter::overwrite(VkDescriptorSet& set)
{
	for (auto& write : writes)
	{
		write.dstSet = set;
	}
	vkUpdateDescriptorSets(pool.sveDevice.device(), writes.size(), writes.data(), 0, nullptr);
}