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
		//glfwPollEvents();
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

void SVEApp::loadGameObjects()
{
	std::vector<SVEModel::Vertex> vertices{
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

	gameObjects.push_back(std::move(triangle));
}

void SVEApp::loadModels()
{
	std::vector<SVEModel::Vertex> vertices{
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}} };
	sveModel = std::make_unique<SVEModel>(sveDevice, vertices);
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