#include "eepch.h"
#include "sceneSerialization.h"
#include "core/common/engineConstants.h"
#include "../ecs/core/ECSManager.h"
#include "utils/text/jsonUtils.h"
#include "../ecs/components/componentRegistry.h"
#include "sceneManager.h"
#include "core/common/engineError.h"
#include "../ecs/components/metadataComponent.h"
#include "../ecs/components/sceneGraphComponent.h"
#include "utils/core/clipboard.h"

namespace emerald {
	struct SerializedComponent {
		std::string type = "";
		UUID uuid = UUID();
		nlohmann::json data = "";

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedComponent, type, uuid, data)
	};

	struct SerializedEntity {
		UUID uuid = UUID();
		std::vector<UUID> components;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedEntity, uuid, components)
	};

	struct SerializedScene {
		std::vector<SerializedEntity> entities;
		std::vector<SerializedComponent> components;
		UUID sceneRootID;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(SerializedScene, entities, components, sceneRootID)
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
				serializedEntity.components.push_back(component->getUUID());
				allComponents.push_back(component);
			}

			serializedScene.entities.push_back(serializedEntity);
		}

		serializedScene.sceneRootID = scene->getRootNode()->getUUID();
		serializedScene.components.reserve(allComponents.size());
		for (auto& component : allComponents) {
			SerializedComponent serializedComponent;
			serializedComponent.type = component->getClassName();
			serializedComponent.uuid = component->getUUID();
			serializedComponent.data = component->serialize();
			serializedScene.components.push_back(serializedComponent);
		}

		nlohmann::json j;
		j[VERSION_KEY] = EngineConstants::VERSION_SCENESERIALIZATION;
		j[SCENE_KEY] = serializedScene;

		jsonUtils::saveToFile(j, path);
	}

	Ref<Scene> SceneSerialization::deserializeScene(const std::filesystem::path& path) {
		Ref<Scene> scene = nullptr;
		try {
			nlohmann::json j = jsonUtils::readFromFile(path);

			uint32_t version = jsonUtils::deserializeRequiredValue<uint32_t>(j, VERSION_KEY);
			if (version != EngineConstants::VERSION_SCENESERIALIZATION) {
				throw jsonUtils::VersionMismatchError(EngineConstants::VERSION_SCENESERIALIZATION, version);
			}

			SerializedScene serializedScene = jsonUtils::deserializeRequiredValue<SerializedScene>(j, SCENE_KEY);

			ECSManager::ECS().clear();

			std::unordered_map<UUID, SerializedComponent> componentMap;
			componentMap.reserve(serializedScene.components.size());
			for (auto& comp : serializedScene.components) {
				componentMap[comp.uuid] = comp;
			}

			std::unordered_map<UUID, Entity> entityMap;
			for (auto& serializedEntity : serializedScene.entities) {
				entityMap.insert({ serializedEntity.uuid, ECSManager::ECS().createEntity() });
			}

			std::unordered_map<Component*, nlohmann::json> createdComponents;
			createdComponents.reserve(serializedScene.components.size());

			for (auto& serializedEntity : serializedScene.entities) {
				for (auto compHash : serializedEntity.components) {
					auto it = componentMap.find(compHash);
					if (it == componentMap.end()) {
						Log::warn("No serialized component data found for hash {}", compHash.toString());
						continue;
					}

					SerializedComponent& serializedComponent = it->second;
					const nlohmann::json& serializedComponentData = serializedComponent.data;

					Entity entity = entityMap[serializedEntity.uuid];
					auto reflectionData = Reflection::findByName(serializedComponent.type);

					Component* component = ComponentRegistry::create(reflectionData->m_type, entityMap[serializedEntity.uuid], serializedComponent.uuid);
					if (!component) {
						Log::error("Failed to create component of type '{}'", serializedComponent.type);
						continue;
					}
					createdComponents.insert({ component, serializedComponentData });
				}
			}

			scene = Ref<Scene>::create(path, serializedScene.sceneRootID);
			SceneManager::setActiveScene(scene);

			for (auto& [component, data] : createdComponents) {
				component->deserialize(data);
			}

			SceneGraphComponent* s = scene->getRootNode();
			auto loop = [](SceneGraphComponent* s, auto& loop, int depth) -> void {
				auto a = s->getChildren();
				for (auto& e : a) {
					auto* z = ECSManager::ECS().getComponent<MetadataComponent>(e->m_entity);
					std::string indent(depth * 2, ' '); 
					Log::info("{}{}", indent, z->getName().c_str());
					loop(e, loop, depth++);
				}
			};

			loop(s, loop, 0);
		} catch (std::exception ex) {
			EngineError::raise(Severity::ERROR, "Scene deserialization error", ex.what());
		}
		return scene;
	}
}