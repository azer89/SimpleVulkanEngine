#include "SVEApp.h"
#include "SVECamera.h"
#include "SVEDescriptorWriter.h"
#include "SVETexture.h"
#include "SVEGAmeObject.h"

#include "SimpleRenderSystem.h"
#include "CircleBillboardRenderSystem.h"
#include "UserInputController.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>
#include <iostream>

SVEApp::SVEApp()
{
	sveWindow = std::make_shared<SVEWindow>(WIDTH, HEIGHT, TITLE);
	sveDevice = std::make_shared<SVEDevice>(sveWindow);
	sveRenderer = std::make_unique<SVERenderer>(sveWindow, sveDevice);

	auto globalPoolBuilder =
		SVEDescriptorPool::Builder(sveDevice)
		.setMaxSets(SVESwapChain::MAX_FRAMES_IN_FLIGHT)
		// UBO
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SVESwapChain::MAX_FRAMES_IN_FLIGHT)
		// Image Sampler
		.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SVESwapChain::MAX_FRAMES_IN_FLIGHT);
	globalPool = globalPoolBuilder.build();
	loadGameObjects();
}

SVEApp::~SVEApp()
{
}

void SVEApp::init()
{

}

void SVEApp::run()
{
	std::vector<std::unique_ptr<SVEBuffer>> uboBuffers(SVESwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < uboBuffers.size(); i++)
	{
		uboBuffers[i] = std::make_unique<SVEBuffer>(
			sveDevice,
			sizeof(GlobalUbo),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		uboBuffers[i]->map();
	}

	auto globalSetLayout =
		SVEDescriptorSetLayout::Builder(sveDevice)
		// UBO
		.addBinding(
			0,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) // VK_SHADER_STAGE_ALL_GRAPHICS
		// Image Sampler
		.addBinding(
			1,
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			VK_SHADER_STAGE_FRAGMENT_BIT)
		.build();

	std::vector<VkDescriptorSet> globalDescriptorSets(SVESwapChain::MAX_FRAMES_IN_FLIGHT);
	SVETexture simpleTexture{ sveDevice, TEXTURE_PATH };
	VkDescriptorImageInfo imageInfo = simpleTexture.descriptorImageInfo();

	for (int i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		SVEDescriptorWriter(*globalSetLayout, *globalPool)
			.writeBuffer(0, &bufferInfo)
			.writeImage(1, &imageInfo)
			.build(globalDescriptorSets[i]);
	}

	SimpleRenderSystem simpleRenderSystem{
		sveDevice,
		sveRenderer->getSwapChainRenderPass(),
		globalSetLayout->getDescriptorSetLayout()
	};

	CircleBillboardRenderSystem cbRenderSystem
	{
		sveDevice,
		sveRenderer->getSwapChainRenderPass(),
		globalSetLayout->getDescriptorSetLayout()
	};

	SVECamera camera{};
	auto viewerObject = SVEGameObject::createGameObject();
	viewerObject.transform.translation = { -3.39563,-3.55833,-0.955367 };
	viewerObject.transform.rotation = { -0.703289,1.1799,0 };
	UserInputController cameraController{};

	while (!sveWindow->shouldClose())
	{
		glfwPollEvents();

		cameraController.update(sveWindow->getGLFWwindow(), viewerObject);
		camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		float aspect = sveRenderer->getAspectRatio();
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = sveRenderer->beginFrame())
		{
			int frameIndex = sveRenderer->getFrameIndex();
			FrameInfo frameInfo{ frameIndex,
				cameraController.getDeltaTime(),
				commandBuffer,
				camera,
				globalDescriptorSets[frameIndex],
				gameObjects };

			// ubo
			GlobalUbo ubo = createUbo(frameInfo, camera);
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			sveRenderer->beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.render(frameInfo);
			cbRenderSystem.render(frameInfo);
			sveRenderer->endSwapChainRenderPass(commandBuffer);
			sveRenderer->endFrame();
		}
	}

	vkDeviceWaitIdle(sveDevice->device());
}

void SVEApp::loadGameObjects()
{
	std::shared_ptr<SVEModel> sveModel = SVEModel::createModelFromFile(sveDevice, DRAGON_MODEL_PATH);
	auto dragon = SVEGameObject::createGameObject();
	dragon.model = sveModel;
	dragon.transform.translation = { .0f, 0.0f, .0f };
	dragon.transform.scale = { 1.f, -1.f, 1.f };
	gameObjects.emplace(dragon.getId(), std::move(dragon));

	std::shared_ptr<SVEModel> floorModel = SVEModel::createModelFromFile(sveDevice, QUAD_MODEL_PATH);
	auto floor = SVEGameObject::createGameObject();
	floor.model = floorModel;
	floor.transform.translation = { 0.f, 0.0f, 0.f };
	floor.transform.scale = { 3.f, 1.f, 3.f };
	addGameObjectToMap(floor);

	std::vector<glm::vec3> lightColors{
	  {1.f, .1f, .1f},
	  {.1f, .1f, 1.f},
	  {.1f, 1.f, .1f},
	  {1.f, 1.f, .1f},
	  {.1f, 1.f, 1.f},
	  {1.f, 1.f, 1.f}  //
	};

	for (int i = 0; i < lightColors.size(); i++)
	{
		auto pointLight = SVEGameObject::makePointLight(2.2f);
		pointLight.color = lightColors[i];
		auto rotateLight = glm::rotate(
			glm::mat4(1.f),
			(i * glm::two_pi<float>()) / lightColors.size(),
			{ 0.f, -1.f, 0.f });
		pointLight.transform.translation = glm::vec3(rotateLight * glm::vec4(-2.f, -1.f, -2.f, 1.f));
		addGameObjectToMap(pointLight);
	}

	/*sveModel = SVEModel::createModelFromFile(sveDevice, "models/smooth_vase.obj");
	auto smoothVase = SVEGameObject::createGameObject();
	smoothVase.model = sveModel;
	smoothVase.transform.translation = { .5f, .5f, 2.5f };
	smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects.push_back(std::move(smoothVase));*/

	std::cout << "Number of game objects = " << gameObjects.size() << '\n';
}

GlobalUbo SVEApp::createUbo(const FrameInfo& frameInfo, const SVECamera& camera)
{
	GlobalUbo ubo;

	ubo.projection = camera.getProjection();
	ubo.view = camera.getView();
	ubo.inverseView = camera.getInverseView();

	auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.deltaTime, { 0.f, -1.f, 0.f });
	int lightIndex = 0;
	for (auto& kv : frameInfo.gameObjects)
	{
		auto& obj = kv.second;
		if (obj.pointLight == nullptr)
		{
			continue;
		}

		assert(lightIndex < MAX_LIGHTS && "Point lights exceed maximum specified");

		// update light position
		obj.transform.translation = glm::vec3(rotateLight * glm::vec4(obj.transform.translation, 1.f));

		// copy light to ubo
		ubo.pointLights[lightIndex].position = glm::vec4(obj.transform.translation, 1.f);
		ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->lightIntensity);

		lightIndex += 1;
	}

	return ubo;
}

void SVEApp::addGameObjectToMap(SVEGameObject& go)
{
	assert(gameObjects.size() < MAX_OBJECTS && "Cannot add game object anymore");
	gameObjects.emplace(go.getId(), std::move(go));
}