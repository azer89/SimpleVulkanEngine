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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

void SVEWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
{
	if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface");
	}
}

void SVEWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto lveWindow = reinterpret_cast<SVEWindow*>(glfwGetWindowUserPointer(window));
	lveWindow->framebufferResized = true;
	lveWindow->width = width;
	lveWindow->height = height;
}