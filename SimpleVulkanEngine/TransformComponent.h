#ifndef TRANSFORM_2D_COMPONENT_H
#define TRANSFORM_2D_COMPONENT_H

#include "SVEModel.h"
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent
{
	//glm::vec2 translation{};  // (position offset)
	//glm::vec2 scale{ 1.f, 1.f };
	//float rotation;
	glm::vec3 translation{ 0.f, 0.f, 0.f };
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{ 0.f, 0.f, 0.f };

	/*glm::mat2 mat2()
	{
		const float s = glm::sin(rotation);
		const float c = glm::cos(rotation);
		glm::mat2 rotMatrix{ {c, s}, {-s, c} };
		glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
		return rotMatrix * scaleMat;
	}*/

	// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
	// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
	// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

#endif
