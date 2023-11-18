#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <glm/glm.hpp>

struct PointLight
{
	glm::vec4 position{};	// ignore w
	glm::vec4 color{};		// w is intensity
};

// ???
struct PointLightComponent
{
	float lightIntensity = 1.0f;
};

struct CircleBillboardPushConstants
{
	glm::vec4 position{};
	glm::vec4 color{};
	float radius;
};

#endif
