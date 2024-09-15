#include "eepch.h"
#include "entity.h"
#include "scene/sceneManager.h"

namespace emerald {
	Entity Entity::create(const std::string& name) {
		return SceneManager::getActiveScene()->getECS().createEntity(name);
	}
}