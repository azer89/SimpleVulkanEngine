#ifndef GLOBAL_UBO_H
#define GLOBAL_UBO_H

#include <glm/glm.hpp>

#include "PointLight.h"

// TODO don't have to be constants
#define MAX_LIGHTS 10
#define MAX_OBJECTS 20

struct GlobalUbo
{
	glm::mat4 projection{ 1.f };
	glm::mat4 view{ 1.f };
	glm::mat4 inverseView{ 1.f };
	glm::vec4 ambientLightColor{ 1.f, 1.f, 1.f, .02f };  // w is intensity
	PointLight pointLights[MAX_LIGHTS];
	//glm::mat4 modelMatrix[MAX_OBJECTS];
	//glm::mat4 normalMatrix[MAX_OBJECTS];

	/*GlobalUbo()
	{
		for (size_t i = 0; i < MAX_OBJECTS; ++i)
		{
			modelMatrix[i] = glm::mat4{1.0f};
			normalMatrix[i] = glm::mat4{1.0f};
		}
	}

	void UpdateMatrices(uint32_t index, glm::mat4 model, glm::mat4 normal)
	{
		assert(index < MAX_OBJECTS);

		modelMatrix[index] = model;
		normalMatrix[index] = normal;
	}*/
};

#endif
