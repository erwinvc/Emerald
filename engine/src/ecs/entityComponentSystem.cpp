#include "eepch.h"
#include "entityComponentSystem.h"
#include "components/transformComponent.h"
#include "components/metadataComponent.h"
#include "components/sceneGraphComponent.h"
#include "util/uuidGenerator.h"
#include "scene/sceneManager.h"
#include <memory>
#include "componentArray.h"

namespace emerald {
	UUID EntityComponentSystem::getNewEntityID() {
		UUID newID = UUIDGenerator::createFast();
		return newID;
	}

	UUID EntityComponentSystem::createEntityFromID(UUID ID, const std::string& name, bool isRootEntity) {
		m_entities.push_back(ID);

		addComponent<TransformComponent>(ID);
		addComponent<MetadataComponent>(ID, name);
		//const WeakRef<SceneGraphComponent>& sgc = addComponent<SceneGraphComponent>(ID);
		SceneGraphComponent* sgc = addComponent<SceneGraphComponent>(ID);
		if (!isRootEntity) SceneManager::getActiveScene()->getRootNode()->addChild(sgc);
		setEntityEnabled(ID, true);
		return Entity(ID);
	}

	UUID EntityComponentSystem::createEntity(const std::string& name, bool isRootEntity) {
		return createEntityFromID(getNewEntityID(), name, isRootEntity);
	}
}