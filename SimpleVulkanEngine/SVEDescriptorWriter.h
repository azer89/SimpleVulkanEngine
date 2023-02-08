#ifndef SVE_DESCRIPTOR_WRITER_H
#define SVE_DESCRIPTOR_WRITER_H

#include "SVEDescriptorSetLayout.h"
#include "SVEDescriptorPool.h"

class SVEDescriptorWriter
{
public:
	SVEDescriptorWriter(SVEDescriptorSetLayout& setLayout, SVEDescriptorPool& pool);

	SVEDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
	SVEDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

	bool build(VkDescriptorSet& set);
	void overwrite(VkDescriptorSet& set);

private:
	SVEDescriptorSetLayout& setLayout;
	SVEDescriptorPool& pool;
	std::vector<VkWriteDescriptorSet> writes;
};

#endif