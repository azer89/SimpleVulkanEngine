#ifndef GLOBAL_UBO_H
#define GLOBAL_UBO_H

#include <glm/glm.hpp>

struct GlobalUbo
{
	glm::mat4 projectionView{ 1.f };
	glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
};

#endif
