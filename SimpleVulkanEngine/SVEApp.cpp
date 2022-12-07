#include "SVEApp.h"

#include <array>
#include <stdexcept>

SVEApp::SVEApp()
{
	createPipelineLayout();
	createPipeline();
	createCommandBuffers();
}

SVEApp::~SVEApp()
{
	vkDestroyPipelineLayout(sveEngine.device(), pipelineLayout, nullptr);
}

void SVEApp::Run()
{
	while (!sveWindow.ShouldClose())
	{
		glfwPollEvents();
		drawFrame();
	}
	vkDeviceWaitIdle(sveEngine.device());
}

void SVEApp::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;
	pipelineLayoutInfo.pSetLayouts = nullptr;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(sveEngine.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create pipeline layout!");
	}
}

void SVEApp::createPipeline()
{
	PipelineConfigInfo pipelineConfig{};
	SVEPipeline::defaultPipelineConfigInfo(
		pipelineConfig,
		sveSwapChain.width(),
		sveSwapChain.height());
	pipelineConfig.renderPass = sveSwapChain.getRenderPass();
	pipelineConfig.pipelineLayout = pipelineLayout;
	svePipeline = std::make_unique<SVEPipeline>(
		sveEngine,
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH,
		pipelineConfig);
}

void SVEApp::createCommandBuffers()
{
	commandBuffers.resize(sveSwapChain.imageCount());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = sveEngine.getCommandPool();
	allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	if (vkAllocateCommandBuffers(sveEngine.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	for (int i = 0; i < commandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = sveSwapChain.getRenderPass();
		renderPassInfo.framebuffer = sveSwapChain.getFrameBuffer(i);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = sveSwapChain.getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues;
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		svePipeline->bind(commandBuffers[i]);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);
		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void SVEApp::drawFrame()
{
	uint32_t imageIndex;
	auto result = sveSwapChain.acquireNextImage(&imageIndex);
	if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	result = sveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image!");
	}
}