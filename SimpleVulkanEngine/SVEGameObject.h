#ifndef SVE_GAME_OBJECT_H
#define SVE_GAME_OBJECT_H

#include <memory>
#include <unordered_map>

#include "SVEModel.h"
#include "TransformComponent.h"
#include "PointLight.h"

class SVEGameObject
{
public:
	using id_t = unsigned int;
	using Map = std::unordered_map<id_t, SVEGameObject>;

	SVEGameObject(const SVEGameObject&) = delete;
	SVEGameObject& operator=(const SVEGameObject&) = delete;
	SVEGameObject(SVEGameObject&&) = default;
	SVEGameObject& operator=(SVEGameObject&&) = default;

	id_t getId() { return id; }

	glm::vec3 color{};
	TransformComponent transform{};

	// Optional pointer components
	std::shared_ptr<SVEModel> model{};
	std::unique_ptr<PointLightComponent> pointLight = nullptr;

public:
	static SVEGameObject createGameObject();
	static SVEGameObject makePointLight(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

private:
	SVEGameObject(id_t objId) : id{ objId } {}

	id_t id = -1;
};

#endif
