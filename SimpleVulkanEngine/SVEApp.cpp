#include "SVEApp.h"

SVEApp::SVEApp()
{
	createPipeline();
}

SVEApp::~SVEApp()
{

}

void SVEApp::Run()
{
	while (!sveWindow.ShouldClose())
	{
		glfwPollEvents();
	}
}

void SVEApp::createPipelineLayout()
{
}

void SVEApp::createPipeline()
{
	PipelineConfigInfo pipelineConfig{};
	SVEPipeline::defaultPipelineConfigInfo(pipelineConfig, WIDTH, HEIGHT);
	//svePipeline = std::make_unique<SVEPipeline>(sveEngine, VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH, pipelineConfig);
}

void SVEApp::createCommandBuffers()
{
}

void SVEApp::drawFrame()
{
}