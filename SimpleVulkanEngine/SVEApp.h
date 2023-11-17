#ifndef SVE_APP_H
#define SVE_APP_H

#include "SVEDevice.h"
#include "SVEModel.h" // not used
#include "SVEGameObject.h"
#include "SVERenderer.h"
#include "SVEWindow.h" 
#include "SVEBuffer.h"
#include "GlobalUBO.h"
#include "FrameInfo.h"
#include "SVETexture.h"
#include "SVEDescriptorPool.h"
#include "SVEDescriptorWriter.h"
#include "SimpleRenderSystem.h"
#include "CircleBillboardRenderSystem.h"

#include "AppBase.h"

// std
#include <memory>
#include <vector>

class SVEApp : AppBase
{
public:
	//static constexpr const char* TITLE = "Simple Vulkan";
	//static constexpr const char* VERTEX_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/vert.spv";
	static constexpr const char* FRAGMENT_SHADER_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Shaders/frag.spv";
	static constexpr int WIDTH = 800;
	//static constexpr int HEIGHT = 600;

	// Models
	static constexpr const char* SMOOTH_VASE_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/smooth_vase.obj";
	static constexpr const char* DRAGON_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/dragon.obj";
	static constexpr const char* KITTEN_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/kitten.obj";
	static constexpr const char* QUAD_MODEL_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Models/quad.obj";

	// Texture, only supports a single image for now
	static constexpr const char* TEXTURE_PATH = "C:/Users/azer/workspace/SimpleVulkanEngine/Textures/texture.jpg";

	SVEApp();
	~SVEApp();

	void Init();
	void Run() override;

private:
	void LoadGameObjects();
	void AddGameObjectToMap(SVEGameObject& go);
	GlobalUbo CreateUbo(const FrameInfo& frameInfo);

	std::unique_ptr<SVEDescriptorPool> globalPool;
	std::unique_ptr<SVEDescriptorSetLayout> globalSetLayout;
	std::unique_ptr<SimpleRenderSystem> simpleRenderSystem;
	std::unique_ptr<CircleBillboardRenderSystem> cbRenderSystem;
	std::unique_ptr<SVETexture> simpleTexture;

	std::vector<std::unique_ptr<SVEBuffer>> uboBuffers;
	std::vector<VkDescriptorSet> globalDescriptorSets;

	SVEGameObject::Map gameObjects;
};

#endif