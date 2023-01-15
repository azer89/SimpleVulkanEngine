#ifndef SVE_GAME_OBJECT_H
#define SVE_GAME_OBJECT_H

#include "SVEModel.h"
#include "TransformComponent.h"

#include <memory>

class SVEGameObject
{
public:
	using id_t = unsigned int;

	static SVEGameObject createGameObject()
	{
		static id_t currentId = 0;
		return SVEGameObject{ currentId++ };
	}

	SVEGameObject(const SVEGameObject&) = delete;
	SVEGameObject& operator=(const SVEGameObject&) = delete;
	SVEGameObject(SVEGameObject&&) = default;
	SVEGameObject& operator=(SVEGameObject&&) = default;

	id_t getId() { return id; }

	std::shared_ptr<SVEModel> model{};
	glm::vec3 color{};
	TransformComponent transform{};

private:
	SVEGameObject(id_t objId) : id{ objId } {}

	id_t id;
};

#endif
