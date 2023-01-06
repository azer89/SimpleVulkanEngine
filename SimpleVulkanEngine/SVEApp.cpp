#include "SVEApp.h"
#include "SimplePushConstantData.h"

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
	createPipelineLayout();
	recreateSwapChain();
	createCommandBuffers();
}

SVEApp::~SVEApp() 
{ 
	vkDestroyPipelineLayout(sveDevice.device(), pipelineLayout, nullptr); 
}

void SVEApp::run()
{
	while (!sveWindow.shouldClose())
	{
		glfwPollEvents();
		drawFrame();
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
	triangle.transform2d.translation.x = .2f;
	triangle.transform2d.scale = { 2.f, .5f };
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

void SVEApp::createPipelineLayout()
{
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
	if (vkCreatePipelineLayout(sveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
		VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void SVEApp::recreateSwapChain()
{
	auto extent = sveWindow.getExtent();
	while (extent.width == 0 || extent.height == 0)
	{
		extent = sveWindow.getExtent();
		glfwWaitEvents();
	}
	vkDeviceWaitIdle(sveDevice.device());

	if (sveSwapChain == nullptr)
	{
		sveSwapChain = std::make_unique<SVESwapChain>(sveDevice, extent);
	}
	else
	{
		sveSwapChain = std::make_unique<SVESwapChain>(sveDevice, extent, std::move(sveSwapChain));
		if (sveSwapChain->imageCount() != commandBuffers.size())
		{
			freeCommandBuffers();
			createCommandBuffers();
		}
	}

	createPipeline();
}

void SVEApp::createPipeline()
{
	assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
	assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

	PipelineConfigInfo pipelineConfig{};
	SVEPipeline::defaultPipelineConfigInfo(pipelineConfig);
	pipelineConfig.renderPass = sveSwapChain->getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	svePipeline = std::make_unique<SVEPipeline>(
		sveDevice,
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH,
		pipelineConfig);
}

void SVEApp::createCommandBuffers()
{
	commandBuffers.resize(sveSwapChain->imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = sveDevice.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(sveDevice.device(), &allocInfo, commandBuffers.data()) !=
		VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}

void SVEApp::freeCommandBuffers()
{
	vkFreeCommandBuffers(
		sveDevice.device(),
		sveDevice.getCommandPool(),
		static_cast<uint32_t>(commandBuffers.size()),
		commandBuffers.data());
	commandBuffers.clear();
}

void SVEApp::recordCommandBuffer(int imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = sveSwapChain->getRenderPass();
	renderPassInfo.framebuffer = sveSwapChain->getFrameBuffer(imageIndex);

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = sveSwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(sveSwapChain->getSwapChainExtent().width);
	viewport.height = static_cast<float>(sveSwapChain->getSwapChainExtent().height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	VkRect2D scissor{ {0, 0}, sveSwapChain->getSwapChainExtent() };
	vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
	vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

	/*svePipeline->bind(commandBuffers[imageIndex]);
	sveModel->bind(commandBuffers[imageIndex]);
	//sveModel->draw(commandBuffers[imageIndex]);
	for (int j = 0; j < 4; j++)
	{
		SimplePushConstantData push{};
		push.offset = { -0.5f + frame * 0.0002f, -0.4f + j * 0.25f };
		push.color = { 0.0f, 0.0f, 0.2f + 0.2f * j };

		vkCmdPushConstants(
			commandBuffers[imageIndex],
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);
		sveModel->draw(commandBuffers[imageIndex]);
	}*/
	renderGameObjects(commandBuffers[imageIndex]);

	vkCmdEndRenderPass(commandBuffers[imageIndex]);
	if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer!");
	}
}

void SVEApp::drawFrame()
{
	uint32_t imageIndex;
	auto result = sveSwapChain->acquireNextImage(&imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	recordCommandBuffer(imageIndex);
	result = sveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
		sveWindow.wasWindowResized())
	{
		sveWindow.resetWindowResizedFlag();
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void SVEApp::renderGameObjects(VkCommandBuffer commandBuffer)
{
	svePipeline->bind(commandBuffer);

	for (auto& obj : gameObjects)
	{
		obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());

		SimplePushConstantData push{};
		push.offset = obj.transform2d.translation;
		push.color = obj.color;
		push.transform = obj.transform2d.mat2();

		vkCmdPushConstants(
			commandBuffer,
			pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&push);
		obj.model->bind(commandBuffer);
		obj.model->draw(commandBuffer);
	}
}