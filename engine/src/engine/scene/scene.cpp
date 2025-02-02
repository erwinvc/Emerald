#include "eepch.h"
#include "scene.h"
#include "engine/ecs/components/transformComponent.h"
#include "engine/ecs/components/sceneGraphComponent.h"
#include "sceneSerialization.h"
#include "../../editor/src/core/projectManager.h"
#include "utils/system/fileSystem.h"

namespace emerald {
	Scene::Scene(const std::filesystem::path& path, UUID sceneRootID) : m_name(path.stem().string()), m_path(path) {
		if (m_name.empty()) m_name = "Untitled scene";

		if (sceneRootID) {
			m_sceneRoot = ECSManager::ECS().getComponentByID<SceneGraphComponent>(sceneRootID);
		} else {
			Entity root = ECSManager::ECS().createEntity(m_name, true);
			m_sceneRoot = ECSManager::ECS().getComponent<SceneGraphComponent>(root);
		}

		auto& currentProject = ProjectManager::getCurrentProject();
		auto& data = currentProject.getData();
		data.lastScenePath = m_path;
		currentProject.save();
	}

	void Scene::load() {

	}
	void Scene::save() {
		if (!m_isDirty) return;
		if (m_path.empty()) {
			m_path = FileSystem::saveFileDialog({ L"Emerald Scene Files", L"*.scene" });
		}
		if (!m_path.empty()) {
			m_name = m_path.stem().string();
			SceneSerialization::serializeScene(m_path, Ref<Scene>(this));
			m_isDirty = false;
		}
	}

	void Scene::update(Timestep ts) {}
	void Scene::getActiveCamera() {}

	void Scene::updateTransformsRecursively(SceneGraphComponent* node, const glm::mat4& parentTransform) {
		TransformComponent* transform = ECSManager::ECS().getComponent<TransformComponent>(node->m_entity);
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