#ifndef SVE_RENDER_PASS_H
#define SVE_RENDER_PASS_H

#include "SVEDevice.h"

class SVERenderPass
{
public:
	SVERenderPass(SVEDevice& device);
	~SVERenderPass();

	SVERenderPass(const SVERenderPass&) = delete;
	SVERenderPass operator=(const SVERenderPass&) = delete;

private:
	SVEDevice& sveDevice;
};

#endif