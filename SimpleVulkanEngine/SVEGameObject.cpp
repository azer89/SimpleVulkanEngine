#include "SVEGameObject.h"

SVEGameObject SVEGameObject::createGameObject()
{
	static id_t currentId = 0;
	return SVEGameObject{ currentId++ };
}

SVEGameObject SVEGameObject::makePointLight(float intensity, float radius, glm::vec3 color)
{
	SVEGameObject gameObj = SVEGameObject::createGameObject();
	gameObj.color = color;
	gameObj.transform.scale.x = radius;
	gameObj.pointLight = std::make_unique<PointLightComponent>();
	gameObj.pointLight->lightIntensity = intensity;
	return gameObj;
}