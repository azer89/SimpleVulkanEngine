#ifndef USER_INPUT_CONTROLLER_H
#define USER_INPUT_CONTROLLER_H

#include "SVEGameObject.h"
#include "SVEWindow.h"

#include <chrono>

class UserInputController
{
public:
	using Time = std::chrono::steady_clock::time_point;

	enum KeyMappings
	{
		moveLeft = GLFW_KEY_A,
		moveRight = GLFW_KEY_D,
		moveForward = GLFW_KEY_W,
		moveBackward = GLFW_KEY_S,
		moveUp = GLFW_KEY_E,
		moveDown = GLFW_KEY_Q,
		lookLeft = GLFW_KEY_LEFT,
		lookRight = GLFW_KEY_RIGHT,
		lookUp = GLFW_KEY_UP,
		lookDown = GLFW_KEY_DOWN,
	};

	UserInputController();
	~UserInputController();

	UserInputController(GLFWwindow* window);

	void update(GLFWwindow* window, SVEGameObject& gameObject);

	float getDeltaTime() const
	{
		return deltaTime;
	}

private:
	void updateTime();

private:
	float moveSpeed{ 3.f };
	float lookSpeed{ 1.5f };

	Time currentTime;
	float deltaTime;
};

#endif
