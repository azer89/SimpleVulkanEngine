#ifndef SVE_FRAME_BUFFER_H
#define SVE_FRAME_BUFFER_H

#include "SVEDevice.h"

class SVEFrameBuffer
{
public:
	SVEFrameBuffer(SVEFrameBuffer& device);
	~SVEFrameBuffer();

	SVEFrameBuffer(const SVEFrameBuffer&) = delete;
	SVEFrameBuffer operator=(const SVEFrameBuffer&) = delete;

private:
	SVEDevice& sveDevice;
};

#endif