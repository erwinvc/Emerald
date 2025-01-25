#include "eepch.h"
#include "sceneSerialization.h"
#include "core/common/engineConstants.h"
#include "../ecs/core/ECSManager.h"
#include "utils/text/jsonUtils.h"

namespace emerald {
	struct SerializedComponent {
		std::string type = "";
		uint64_t hash = 0;
		nlohmann::json data = "";

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedComponent, type, hash, data)
	};

	struct SerializedEntity {
		UUID uuid = UUID();
		std::vector<uint64_t> components;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedEntity, uuid, components)
	};

	struct SerializedScene {
		std::vector<SerializedEntity> entities;
		std::vector<SerializedComponent> components;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedScene, entities, components)
	};

	void SceneSerialization::serializeScene(const std::filesystem::path& path, const Ref<Scene>& scene) {
		const std::vector<UUID>& entities = ECSManager::ECS().getEntities();
		std::vector<Component*> allComponents;

		SerializedScene serializedScene;

		for (auto& entity : entities) {
			SerializedEntity serializedEntity;
			serializedEntity.uuid = entity;
			const std::vector<Component*>& componentsInEntity = ECSManager::ECS().getAllComponents(entity);

			for (auto& component : componentsInEntity) {
				serializedEntity.components.push_back(component->getHash());
				allComponents.push_back(component);
			}

			serializedScene.entities.push_back(serializedEntity);
		}

		serializedScene.components.reserve(allComponents.size());
		for (auto& component : allComponents) {
			SerializedComponent serializedComponent;
			serializedComponent.type = component->getComponentTypeInfo().name;
			serializedComponent.hash = component->getHash();
			serializedComponent.data = component->serialize();
			serializedScene.components.push_back(serializedComponent);
		}

		nlohmann::json j;
		j[VERSION_KEY] = editorConstants::SERIALIZATIONVERSION;
		j[SCENE_KEY] = serializedScene;

		Log::info("{}", j.dump());
	}
	Ref<Scene> SceneSerialization::deserializeScene(const std::filesystem::path& path) {
		nlohmann::json j = jsonUtils::readFromFile(path);

		uint32_t version = jsonUtils::deserializeRequiredValue<uint32_t>(j, VERSION_KEY);
		if (version != editorConstants::SERIALIZATIONVERSION) {
			throw jsonUtils::VersionMismatchError(editorConstants::SERIALIZATIONVERSION, version);
		}

		SerializedScene serializedScene = jsonUtils::deserializeRequiredValue<SerializedScene>(j, SCENE_KEY);

		ECSManager::ECS().clear();

	}
}