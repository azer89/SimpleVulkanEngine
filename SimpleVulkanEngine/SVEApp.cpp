#include "SVEApp.h"
#include "KeyboardMovementController.h"
#include "SVECamera.h"
#include "SimpleRenderSystem.h"

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
	loadGameObjects();
	//loadModels();
}

SVEApp::~SVEApp() 
{ 
}

void SVEApp::run()
{
	SimpleRenderSystem simpleRenderSystem{ sveDevice, sveRenderer.getSwapChainRenderPass() };
	SVECamera camera{};
	// camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
	//camera.setViewTarget(glm::vec3(-1.f, -2.f, -2.f), glm::vec3(0.f, 0.f, 2.5f));
	auto viewerObject = SVEGameObject::createGameObject();
	viewerObject.transform.translation = { 0, -0.5f, -2.0f };
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
		camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

		if (auto commandBuffer = sveRenderer.beginFrame())
		{
			sveRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
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

	std::shared_ptr<SVEModel> sveModel = SVEModel::createModelFromFile(sveDevice, SMOOTH_VASE_MODEL_PATH);
	auto flatVase = SVEGameObject::createGameObject();
	flatVase.model = sveModel;
	flatVase.transform.translation = { .0f, .0f, .0f };
	flatVase.transform.scale = { 3.f, 1.5f, 3.f };
	gameObjects.push_back(std::move(flatVase));

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