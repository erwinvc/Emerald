#include "eepch.h"
#include "entityComponentSystem.h"
#include "components.h"
#include "../scene/sceneManager.h"

namespace emerald {
	uint32_t EntityComponentSystem::getFreeEntityIndex() {
		uint32_t newID = m_nextEntityID++;
		return newID;
	}

	uint32_t EntityComponentSystem::createEntity(uint32_t index, const std::string& name) {
		m_entities.push_back(index);

		addComponent<UUIDComponent>(index);
		addComponent<TransformComponent>(index);
		addComponent<NameComponent>(index, name);
		SceneGraphComponent* sgc = addComponent<SceneGraphComponent>(index);
		SceneManager::getActiveScene()->getRootNode()->addChild(sgc);
		return Entity(index);
	}

	uint32_t EntityComponentSystem::createEntity(const std::string& name) {
		return createEntity(getFreeEntityIndex(), name);
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