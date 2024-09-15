#include "eepch.h"
#include "scene.h"
#include "assets/loaders/modelLoader.h"
#include "ecs/components/transformComponent.h"
#include "ecs/components/nameComponent.h"
#include "ecs/components/meshRendererComponent.h"

namespace emerald {
	Scene::Scene(const std::string name, const std::filesystem::path& scenePath) : m_name(name), m_path(scenePath) {

	}

	void Scene::initialize() {
		m_ecs.registerComponent<SceneGraphComponent>();
		m_ecs.registerComponent<TransformComponent>();
		m_ecs.registerComponent<NameComponent>();
		m_ecs.registerComponent<MeshRendererComponent>();

		Entity root = m_ecs.createEntity(m_name, true);
		m_sceneRoot = m_ecs.getComponent<SceneGraphComponent>(root);

		Entity sponza = Entity::create("sponza");

		SceneGraphComponent* sponzaParent = m_ecs.getComponent<SceneGraphComponent>(sponza);
		TransformComponent* sponzaTransform = m_ecs.getComponent<TransformComponent>(sponza);
		sponzaTransform->setScale(glm::vec3(0.1f));

		ModelLoader loader("I:\\Development\\C++\\EmeraldOldStuff\\res\\sponza\\sponza.obj");
		for (auto& mesh : loader.load()) {
			Entity e = m_ecs.createEntity(mesh->getName());
			SceneGraphComponent* r = m_ecs.getComponent<SceneGraphComponent>(e);
			m_ecs.addComponent<MeshRendererComponent>(e, mesh);
			sponzaParent->addChild(r);
		}
	}
	void Scene::load() {}
	void Scene::save() {}
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