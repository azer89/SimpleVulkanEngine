#include "SVEWindow.h"

#include <stdexcept>

SVEWindow::SVEWindow(int w, int h, std::string name) :
	width{ w },
	height{ h },
	windowName{ name }
{
	initWindow();
}

SVEWindow::~SVEWindow()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

bool SVEWindow::ShouldClose()
{
	return glfwWindowShouldClose(window);
}

void SVEWindow::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // No context
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
}

void SVEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

VkExtent2D SVEWindow::getExtent()
{
	return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}