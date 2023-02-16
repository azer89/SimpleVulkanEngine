#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEDevice.h"
#include "SVEModel.h" // not used
#include "SVEGameObject.h"
#include "SVERenderer.h"
#include "SVEWindow.h" 
#include "SVEBuffer.h"
#include "SVEDescriptorPool.h"
#include "GlobalUBO.h"
#include "FrameInfo.h"

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

	// Models
	static constexpr const char* SMOOTH_VASE_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/smooth_vase.obj";
	static constexpr const char* DRAGON_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/dragon.obj";
	static constexpr const char* QUAD_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/quad.obj";

	// Texture, only supports a single image for now
	static constexpr const char* TEXTURE_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Textures/Texture.jpg";

	SVEApp();
	~SVEApp();

	SVEApp(const SVEApp&) = delete;
	SVEApp& operator=(const SVEApp&) = delete;

	void run();

private:
	//void loadModels(); // not used
	void loadGameObjects();
	GlobalUbo createUbo(const FrameInfo& frameInfo, const SVECamera& camera);
	void addGameObjectToMap(SVEGameObject& go);

	SVEWindow sveWindow{ WIDTH, HEIGHT, TITLE };
	SVEDevice sveDevice{ sveWindow };
	SVERenderer sveRenderer{sveWindow, sveDevice};
	//std::unique_ptr<SVEModel> sveModel; // not used
	std::unique_ptr<SVEDescriptorPool> globalPool{};
	SVEGameObject::Map gameObjects;
};

#endif