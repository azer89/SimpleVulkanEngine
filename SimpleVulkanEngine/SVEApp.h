#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEWindow.h"
#include "SVEPipeline.h"

class SVEApp
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;
	static constexpr const char* TITLE = "Simple Vulkan";
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";

	void Run();

private:
	SVEWindow sveWindow{ WIDTH, HEIGHT, TITLE };
	SVEPipeline svePipeline{ VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH };
};

#endif