#ifndef SIMPLE_PUSH_CONSTANT_DATA_H
#define SIMPLE_PUSH_CONSTANT_DATA_H

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct SimplePushConstantData
{
	glm::mat4 modelMatrix{ 1.f };
	glm::mat4 normalMatrix{ 1.f };
};

#endif
