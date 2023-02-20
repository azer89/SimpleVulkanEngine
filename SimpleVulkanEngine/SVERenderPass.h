#ifndef SVE_RENDER_PASS_H
#define SVE_RENDER_PASS_H

#include "SVEDevice.h"

class SVERenderPass
{
public:
	SVERenderPass(const std::shared_ptr<SVEDevice>& device);
	~SVERenderPass();

	SVERenderPass(const SVERenderPass&) = delete;
	SVERenderPass operator=(const SVERenderPass&) = delete;

	VkRenderPass getRenderPass() const { return renderPass; }

private:
	void init();

	std::shared_ptr<SVEDevice> sveDevice;
	VkRenderPass renderPass;
};

#endif