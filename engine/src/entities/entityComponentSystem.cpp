#include "eepch.h"
#include "entityComponentSystem.h"
#include "components.h"
#include "../scene/sceneManager.h"

namespace emerald {
	uint32_t EntityComponentSystem::getFreeEntityIndex() {
		uint32_t newID = m_nextEntityID++;
		return newID;
	}

	uint32_t EntityComponentSystem::createEntityFromID(uint32_t ID, const std::string& name, bool isRootEntity) {
		m_entities.push_back(ID);

		addComponent<UUIDComponent>(ID);
		addComponent<TransformComponent>(ID);
		addComponent<NameComponent>(ID, name);
		WeakRef<SceneGraphComponent> sgc = addComponent<SceneGraphComponent>(ID);
		if (!isRootEntity) SceneManager::getActiveScene()->getRootNode()->addChild(sgc.lock().raw());
		return Entity(ID);
	}

	uint32_t EntityComponentSystem::createEntity(const std::string& name, bool isRootEntity) {
		return createEntityFromID(getFreeEntityIndex(), name, isRootEntity);
	}

	RTTIType EntityComponentSystem::getComponentRTTIType() const {
		return Component::getStaticClassType();
	};

	bool EntityComponentSystem::isRemovableComponent(RTTIType type) const {
		return !(type == UUIDComponent::getStaticClassType() ||
			type == NameComponent::getStaticClassType() ||
			type == TransformComponent::getStaticClassType() ||
			type == SceneGraphComponent::getStaticClassType());
	}
}