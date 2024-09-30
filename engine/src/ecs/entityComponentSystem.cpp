#include "eepch.h"
#include "entityComponentSystem.h"
#include "scene/sceneManager.h"
#include "util/uuidGenerator.h"
#include "components/transformComponent.h"
#include "components/nameComponent.h"
#include "components/sceneGraphComponent.h"

namespace emerald {
	UUID EntityComponentSystem::getNewEntityID() {
		UUID newID = UUIDGenerator::createFast();
		return newID;
	}

	UUID EntityComponentSystem::createEntityFromID(UUID ID, const std::string& name, bool isRootEntity) {
		m_entities.push_back(ID);

		addComponent<TransformComponent>(ID);
		addComponent<NameComponent>(ID, name);
		const WeakRef<SceneGraphComponent>& sgc = addComponent<SceneGraphComponent>(ID);
		if (!isRootEntity) SceneManager::getActiveScene()->getRootNode()->addChild(sgc.lock().raw());
		return Entity(ID);
	}

	UUID EntityComponentSystem::createEntity(const std::string& name, bool isRootEntity) {
		return createEntityFromID(getNewEntityID(), name, isRootEntity);
	}

	RTTIType EntityComponentSystem::getComponentRTTIType() const {
		return Component::getStaticClassType();
	};

	bool EntityComponentSystem::isRemovableComponent(RTTIType type) const {
		return !(type == NameComponent::getStaticClassType() ||
			type == TransformComponent::getStaticClassType() ||
			type == SceneGraphComponent::getStaticClassType());
	}

	std::unordered_set<emerald::RTTIType> EntityComponentSystem::getAllComponentTypesForEntity(UUID entity) {
		std::unordered_set<emerald::RTTIType> allComponents;
		for (auto& [type, componentArray] : getComponentArrays()) {
			if (componentArray->has(entity)) {
				allComponents.insert(type);
			}
		}
		return allComponents;
	}
}