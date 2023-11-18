#ifndef TRANSFORM_2D_COMPONENT_H
#define TRANSFORM_2D_COMPONENT_H

#include "SVEModel.h"
#include <glm/gtc/matrix_transform.hpp>

struct TransformComponent
{
	glm::vec3 translation{ 0.f, 0.f, 0.f };
	glm::vec3 scale{ 1.f, 1.f, 1.f };
	glm::vec3 rotation{ 0.f, 0.f, 0.f };

	// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
	// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
	// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
	glm::mat4 mat4();
	glm::mat3 normalMatrix();
};

#endif
