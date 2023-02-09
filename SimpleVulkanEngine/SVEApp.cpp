#include "SVEApp.h"
#include "KeyboardMovementController.h"
#include "SVECamera.h"
#include "SimpleRenderSystem.h"
#include "SVEDescriptorWriter.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

SVEApp::SVEApp()
{
	globalPool =
		SVEDescriptorPool::Builder(sveDevice)
		.setMaxSets(SVESwapChain::MAX_FRAMES_IN_FLIGHT)
		.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SVESwapChain::MAX_FRAMES_IN_FLIGHT)
		.build();
	loadGameObjects();
	//loadModels();
}

SVEApp::~SVEApp() 
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
		.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT) // VK_SHADER_STAGE_ALL_GRAPHICS
		.build();

	std::vector<VkDescriptorSet> globalDescriptorSets(SVESwapChain::MAX_FRAMES_IN_FLIGHT);
	for (int i = 0; i < globalDescriptorSets.size(); i++)
	{
		auto bufferInfo = uboBuffers[i]->descriptorInfo();
		SVEDescriptorWriter(*globalSetLayout, *globalPool)
			.writeBuffer(0, &bufferInfo)
			.build(globalDescriptorSets[i]);
	}

	SimpleRenderSystem simpleRenderSystem{ sveDevice, sveRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };
	SVECamera camera{};
	auto viewerObject = SVEGameObject::createGameObject();
	viewerObject.transform.translation = { 0, -1.5f, -2.0f };
	KeyboardMovementController cameraController{};
	auto currentTime = std::chrono::high_resolution_clock::now();

	while (!sveWindow.shouldClose())
	{
		glfwPollEvents();

		auto newTime = std::chrono::high_resolution_clock::now();
		float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
		currentTime = newTime;

		cameraController.moveInPlaneXZ(sveWindow.getGLFWwindow(), frameTime, viewerObject);
		camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

		float aspect = sveRenderer.getAspectRatio();
		// camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);

		if (auto commandBuffer = sveRenderer.beginFrame())
		{
			int frameIndex = sveRenderer.getFrameIndex();
			FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex] };

			// update
			GlobalUbo ubo{};
			ubo.projectionView = camera.getProjection() * camera.getView();
			uboBuffers[frameIndex]->writeToBuffer(&ubo);
			uboBuffers[frameIndex]->flush();

			// render
			sveRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
			sveRenderer.endSwapChainRenderPass(commandBuffer);
			sveRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(sveDevice.device());
}

// temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
std::unique_ptr<SVEModel> createCubeModel(SVEDevice& device, glm::vec3 offset)
{
	SVEModel::Builder modelBuilder{};
	modelBuilder.vertices = {
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
	};
	for (auto& v : modelBuilder.vertices)
	{
		v.position += offset;
	}

	modelBuilder.indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
							12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

	return std::make_unique<SVEModel>(device, modelBuilder);
}

void SVEApp::loadGameObjects()
{
	/*std::vector<SVEModel::Vertex> vertices{
	  {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
	auto lveModel = std::make_shared<SVEModel>(sveDevice, vertices);

	auto triangle = SVEGameObject::createGameObject();
	triangle.model = lveModel;
	triangle.color = { .1f, .8f, .1f };
	triangle.transform2d.translation.x = 0.0f;
	triangle.transform2d.scale = { 1.f, 1.f };
	triangle.transform2d.rotation = .25f * glm::two_pi<float>();

	gameObjects.push_back(std::move(triangle));*/


	/*std::shared_ptr<SVEModel> sveModel = createCubeModel(sveDevice, {.0f, .0f, .0f});
	auto cube = SVEGameObject::createGameObject();
	cube.model = sveModel;
	cube.transform.translation = { .0f, .0f, 2.5f };
	cube.transform.scale = { .5f, .5f, .5f };
	gameObjects.push_back(std::move(cube));*/

	/*std::shared_ptr<SVEModel> sveModel = SVEModel::createModelFromFile(sveDevice, SMOOTH_VASE_MODEL_PATH);
	auto flatVase = SVEGameObject::createGameObject();
	flatVase.model = sveModel;
	flatVase.transform.translation = { .0f, .0f, .0f };
	flatVase.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects.push_back(std::move(flatVase));*/
	std::shared_ptr<SVEModel> sveModel = SVEModel::createModelFromFile(sveDevice, DRAGON_MODEL_PATH);
	auto flatVase = SVEGameObject::createGameObject();
	flatVase.model = sveModel;
	flatVase.transform.translation = { .0f, .0f, .0f };
	flatVase.transform.scale = { 1.f, -1.f, 1.f };
	gameObjects.push_back(std::move(flatVase));

	sveModel = SVEModel::createModelFromFile(sveDevice, QUAD_MODEL_PATH);
	auto floor = SVEGameObject::createGameObject();
	floor.model = sveModel;
	floor.transform.translation = { 0.f, .5f, 0.f };
	floor.transform.scale = { 3.f, 1.f, 3.f };
	gameObjects.push_back(std::move(floor));

	/*sveModel = SVEModel::createModelFromFile(sveDevice, "models/smooth_vase.obj");
	auto smoothVase = SVEGameObject::createGameObject();
	smoothVase.model = sveModel;
	smoothVase.transform.translation = { .5f, .5f, 2.5f };
	smoothVase.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects.push_back(std::move(smoothVase));*/
}

//void SVEApp::loadModels()
//{
	/*std::vector<SVEModel::Vertex> vertices{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
	sveModel = std::make_unique<SVEModel>(sveDevice, vertices);*/

	/*std::vector<SVEModel::Vertex> vertices{
	  {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	  {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	  {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
	auto lveModel = std::make_shared<SVEModel>(sveDevice, vertices);

	auto triangle = SVEGameObject::createGameObject();
	triangle.model = lveModel;
	triangle.color = { .1f, .8f, .1f };
	triangle.transform2d.translation.x = .2f;
	triangle.transform2d.scale = { 2.f, .5f };
	triangle.transform2d.rotation = .25f * glm::two_pi<float>();

	gameObjects.push_back(std::move(triangle));*/
//}