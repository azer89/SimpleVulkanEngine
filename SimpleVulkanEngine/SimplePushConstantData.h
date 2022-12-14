#ifndef SIMPLE_PUSH_CONSTANT_DATA_H
#define SIMPLE_PUSH_CONSTANT_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct SimplePushConstantData
{
	glm::mat2 transform{ 1.f }; // 4 x 4 bytes = 16 bytes
	glm::vec2 offset;
	alignas(16) glm::vec3 color;
};

#endif
