#ifndef APP_BASE_H
#define APP_BASE_H

#include "SVEWindow.h"
#include "SVEDevice.h"
#include "SVERenderer.h"

#include <iostream>
#include <memory>
#include <GLFW/glfw3.h>
#include "Camera.h"

class AppBase
{
public:
	AppBase()
	{
		std::cout << "AppBaseConstructor\n";
	}

protected:
	//std::shared_ptr<SVEWindow> sveWindow;
	//std::shared_ptr<SVEDevice> sveDevice;
	//std::unique_ptr<SVERenderer> sveRenderer;

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	// Camera
	/*std::unique_ptr<Camera> camera;
	float lastX;
	float lastY;
	bool firstMouse;*/
};

#endif
