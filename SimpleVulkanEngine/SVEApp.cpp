#include "SVEApp.h"
#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
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
	while (!sveWindow.shouldClose())
	{
		glfwPollEvents();
		if (auto commandBuffer = sveRenderer.beginFrame())
		{
			sveRenderer.beginSwapChainRenderPass(commandBuffer);
			simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
			sveRenderer.endSwapChainRenderPass(commandBuffer);
			sveRenderer.endFrame();
		}
	}

	vkDeviceWaitIdle(sveDevice.device());
}

// temporary helper function, creates a 1x1x1 cube centered at offset
std::unique_ptr<SVEModel> createCubeModel(SVEDevice& device, glm::vec3 offset)
{
	std::vector<SVEModel::Vertex> vertices{
		// left face (white)
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
		{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
		{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

		// right face (yellow)
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
		{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

		// top face (orange, remember y axis points down)
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
		{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
		{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

		// bottom face (red)
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
		{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
		{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

		// nose face (blue)
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
		{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
		{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

		// tail face (green)
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
		{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
	};
	for (auto& v : vertices)
	{
		v.position += offset;
	}
	return std::make_unique<SVEModel>(device, vertices);
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
	std::shared_ptr<SVEModel> sveModel = createCubeModel(sveDevice, { .0f, .0f, .0f });
	auto cube = SVEGameObject::createGameObject();
	cube.model = sveModel;
	cube.transform.translation = { .0f, .0f, .5f };
	cube.transform.scale = { .5f, .5f, .5f };
	gameObjects.push_back(std::move(cube));
}

void SVEApp::loadModels()
{
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
}