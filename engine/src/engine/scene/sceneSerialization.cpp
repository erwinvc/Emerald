#include "eepch.h"
#include "sceneSerialization.h"
#include "core/common/engineConstants.h"

namespace emerald {
	struct SerializedComponent {
		std::string type;
		uint64_t hash;
		nlohmann::json data;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedComponent, type, hash, data)
	};

	struct SerializedEntity {
		UUID uuid;
		std::vector<uint64_t> components;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedEntity, uuid, components)
	};
	struct SerializedScene {
		std::vector<SerializedEntity> entities;
		std::vector<SerializedComponent> components;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedScene, entities, components)
	};

	void SceneSerialization::serializeScene(const std::filesystem::path& path, const Ref<Scene>& scene) {
		EntityComponentSystem& ecs = scene->getECS();

		const std::vector<UUID>& entities = ecs.getEntities();
		std::vector<Component*> allComponents;

		SerializedScene serializedScene;

		for (auto& entity : entities) {
			SerializedEntity serializedEntity;
			serializedEntity.uuid = entity;
			const std::vector<Component*>& componentsInEntity = ecs.getAllComponents(entity);

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
		j["1version"] = editorConstants::SERIALIZATIONVERSION;
		j["2scene"] = serializedScene;

		Log::info("{}", j.dump());
	}
	Ref<Scene> SceneSerialization::deserializeScene(const std::filesystem::path& path) {
		
	}
}