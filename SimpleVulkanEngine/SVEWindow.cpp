#include "SVEWindow.h"

// std
#include <stdexcept>

SVEWindow::SVEWindow(int w, int h, std::string name) : width{ w }, height{ h }, windowName{ name } {
	initWindow();
}

SVEWindow::~SVEWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void SVEWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void SVEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to craete window surface");
	}
}