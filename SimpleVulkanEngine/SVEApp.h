#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEDevice.h"
#include "SVEModel.h" // not used
#include "SVEGameObject.h"
#include "SVERenderer.h"
#include "SVEWindow.h" 

// std
#include <memory>
#include <vector>

class SVEApp
{
public:
	static constexpr const char* TITLE = "Simple Vulkan";
	static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	SVEApp();
	~SVEApp();

	SVEApp(const SVEApp&) = delete;
	SVEApp& operator=(const SVEApp&) = delete;

	void run();

private:
	void loadModels(); // not used
	void loadGameObjects();

	SVEWindow sveWindow{ WIDTH, HEIGHT, TITLE };
	SVEDevice sveDevice{ sveWindow };
	SVERenderer sveRenderer{sveWindow, sveDevice};
	std::unique_ptr<SVEModel> sveModel; // not used
	std::vector<SVEGameObject> gameObjects;
};

#endif