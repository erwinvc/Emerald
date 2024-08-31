#include "eepch.h"
#include "entityComponentSystem.h"

namespace emerald {
	uint32_t EntityComponentSystem::createEntity(const std::string& name) {
		uint32_t newID = m_nextEntityID++;
		m_entities.push_back(newID);

		addComponent<UUIDComponent>(newID);
		addComponent<TransformComponent>(newID);
		addComponent<NameComponent>(newID, name);
		SceneGraphComponent& sgc = addComponent<SceneGraphComponent>(newID);
		return Entity(newID);
	}
}