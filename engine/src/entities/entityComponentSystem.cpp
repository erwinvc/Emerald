#include "eepch.h"
#include "entityComponentSystem.h"
#include "components.h"
#include "../scene/sceneManager.h"

namespace emerald {
	uint32_t EntityComponentSystem::createEntity(const std::string& name) {
		uint32_t newID = m_nextEntityID++;

		m_entities.push_back(newID);

		addComponent<UUIDComponent>(newID);
		addComponent<TransformComponent>(newID);
		addComponent<NameComponent>(newID, name);
		SceneGraphComponent* sgc = addComponent<SceneGraphComponent>(newID);
		SceneManager::getActiveScene()->getRootNode()->addChild(sgc);
		return Entity(newID);
	}

	RTTIType EntityComponentSystem::getComponentRTTIType() const {
		return Component::getClassType();
	};

	bool EntityComponentSystem::isRemovableComponent(RTTIType type) const {
		return !(type == UUIDComponent::getClassType() ||
			type == NameComponent::getClassType() ||
			type == TransformComponent::getClassType() ||
			type == SceneGraphComponent::getClassType());
	}
}