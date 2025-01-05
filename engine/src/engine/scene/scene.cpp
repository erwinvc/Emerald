#include "eepch.h"
#include "scene.h"
#include "engine/assets/loaders/modelLoader.h"
#include "engine/ecs/components/transformComponent.h"
#include "engine/ecs/components/meshRendererComponent.h"
#include "engine/ecs/components/sceneGraphComponent.h"
#include "sceneSerialization.h"

namespace emerald {
	Scene::Scene(const std::string name, const std::filesystem::path& scenePath) : m_name(name), m_path(scenePath) {
		Entity root = m_ecs.createEntity(m_name, true);
		m_sceneRoot = m_ecs.getComponent<SceneGraphComponent>(root);
	}

	void Scene::load() {}
	void Scene::save() {
		//AssetRegistry::getAssetMetadata()
		SceneSerialization::serializeScene(m_path, Ref<Scene>(this));
	}

	void Scene::update(Timestep ts) {}
	void Scene::getActiveCamera() {}

	void Scene::updateTransformsRecursively(SceneGraphComponent* node, const glm::mat4& parentTransform) {
		TransformComponent* transform = m_ecs.getComponent<TransformComponent>(node->m_entity);
		if (!transform) return;
		transform->updateGlobalTransform(parentTransform);

		const std::vector<SceneGraphComponent*>& children = node->getChildren();
		for (SceneGraphComponent* child : children) {
			updateTransformsRecursively(child, transform->getGlobalTransform());
		}
	}

	void Scene::updateAllTransforms() {
		updateTransformsRecursively(getRootNode(), glm::mat4(1.0f));
	}
}