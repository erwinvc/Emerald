#include "eepch.h"
#include "entity.h"
#include "engine/scene/sceneManager.h"

namespace emerald {
	Entity Entity::create(const std::string& name) {
		return ECSManager::ECS().createEntity(name);
	}
}