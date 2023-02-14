#ifndef GLOBAL_UBO_H
#define GLOBAL_UBO_H

#include <glm/glm.hpp>

#include "PointLight.h"

#define MAX_LIGHTS 6

struct GlobalUbo
{
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 inverseView{ 1.f };
	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
	PointLight pointLights[MAX_LIGHTS];
};

#endif
