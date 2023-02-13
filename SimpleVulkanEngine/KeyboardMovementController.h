#ifndef KEYBOARD_MOVEMENT_CONTROLLER_H
#define KEYBOARD_MOVEMENT_CONTROLLER_H

#include "SVEGameObject.h"
#include "SVEWindow.h"

class KeyboardMovementController
{
public:
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

	KeyboardMovementController(GLFWwindow* window);

	void moveInPlaneXZ(GLFWwindow* window, float dt, SVEGameObject& gameObject);

	float moveSpeed{ 3.f };
	float lookSpeed{ 1.5f };
};

#endif
