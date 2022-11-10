#ifndef SVE_WINDOW_H
#define SVE_WINDOW_H

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif

#include <GLFW/glfw3.h>
#include <string>

class SVEWindow
{
public:
	SVEWindow(int w, int h, std::string name);
	~SVEWindow();

	// Non copyable
	SVEWindow(const SVEWindow&) = delete;
	SVEWindow& operator=(const SVEWindow&) = delete;

	bool ShouldClose();

private:
	void initWindow();

	const int width;
	const int height;

	std::string windowName;
	GLFWwindow* window;
};

#endif