#ifndef SVE_WINDOW_H
#define SVE_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class SVEWindow
{
public:
	SVEWindow(int w, int h, std::string name);
	~SVEWindow();

	SVEWindow(const SVEWindow&) = delete;
	SVEWindow& operator=(const SVEWindow&) = delete;

	bool shouldClose() const { return glfwWindowShouldClose(window); }
	VkExtent2D getExtent() const { return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) }; }
	bool wasWindowResized() const { return framebufferResized; }
	void resetWindowResizedFlag() { framebufferResized = false; }
	GLFWwindow* getGLFWwindow() const { return window; }

	void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	void initWindow();
	
	int width;
	int height;
	bool framebufferResized = false;

	std::string windowName;
	GLFWwindow* window;
};

#endif