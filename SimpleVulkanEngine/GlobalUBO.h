#ifndef GLOBAL_UBO_H
#define GLOBAL_UBO_H

#include <glm/glm.hpp>

struct GlobalUbo
{
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
	glm::vec3 lightPosition{ -1.f };
	alignas(16) glm::vec4 lightColor{ 1.f };  // w is light intensity
};

#endif
