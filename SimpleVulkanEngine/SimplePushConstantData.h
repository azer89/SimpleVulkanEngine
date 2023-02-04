#ifndef SIMPLE_PUSH_CONSTANT_DATA_H
#define SIMPLE_PUSH_CONSTANT_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct SimplePushConstantData
{
	glm::mat4 transform{ 1.f }; // 64 bytes
	//alignas(16) glm::vec3 color;
	glm::mat4 normalMatrix{ 1.f };
};

#endif
