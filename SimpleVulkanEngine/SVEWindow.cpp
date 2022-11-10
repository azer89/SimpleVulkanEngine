#include "SVEWindow.h"

SVEWindow::SVEWindow(int w, int h, std::string name) : 
	width{w}, 
	height{h}, 
	windowName{name} 
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