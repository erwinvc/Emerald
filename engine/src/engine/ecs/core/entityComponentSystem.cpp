#include "eepch.h"
#include "engine/ecs/components/metadataComponent.h"
#include "engine/ecs/components/sceneGraphComponent.h"
#include "engine/ecs/components/transformComponent.h"
#include "engine/scene/sceneManager.h"
#include "entityComponentSystem.h"
#include "utils/uuid/uuidGenerator.h"
#include <memory>

namespace emerald {
	UUID EntityComponentSystem::getNewEntityID() {
		UUID newID = UUIDGenerator::create();
		return newID;
	}

	UUID EntityComponentSystem::createEntityFromID(UUID ID) {
		m_entities.push_back(ID);
		setEntityEnabled(ID, true);
		return Entity(ID);
	}

	UUID EntityComponentSystem::createEntityFromID(UUID ID, const std::string& name, bool isRootEntity) {
		m_entities.push_back(ID);

		addComponent<TransformComponent>(ID);
		addComponent<MetadataComponent>(ID, name);
		SceneGraphComponent* sgc = addComponent<SceneGraphComponent>(ID);
		if (!isRootEntity) SceneManager::getActiveScene()->getRootNode()->addChild(sgc);
		setEntityEnabled(ID, true);

		markSceneDirty();
		return Entity(ID);
	}

	UUID EntityComponentSystem::createEntity(const std::string& name, bool isRootEntity) {
		return createEntityFromID(getNewEntityID(), name, isRootEntity);
	}

	UUID EntityComponentSystem::createEntity() {
		return createEntityFromID(getNewEntityID());
	}

	void EntityComponentSystem::markSceneDirty() {
		SceneManager::markSceneDirty();
	}
}