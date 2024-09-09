#include "eepch.h"
#include "scene.h"
#include "entities/components.h"
#include "assets/loaders/modelLoader.h"

namespace emerald {
	Scene::Scene(const std::string name, const std::filesystem::path& scenePath) : m_name(name), m_path(scenePath) {

	}

	void Scene::initialize() {
		m_ecs.registerComponent<SceneGraphComponent>();
		m_ecs.registerComponent<TransformComponent>();
		m_ecs.registerComponent<NameComponent>();
		m_ecs.registerComponent<UUIDComponent>();
		m_ecs.registerComponent<MeshRendererComponent>();

		Entity root = m_ecs.createEntity(m_name, true);
		m_sceneRoot = m_ecs.getComponent<SceneGraphComponent>(root);
		
		Entity sponza = Entity::create("sponza");

		Entity test1 = Entity::create("test1");
		Entity test2 = Entity::create("test2");
		Entity test3 = Entity::create("test3");
		Entity test4 = Entity::create("test4");
		Entity test5 = Entity::create("test5");
		Entity::create("test6");
		Entity::create("test7");
		Entity::create("test8");
		Entity::create("test9");
		Entity::create("test10");

		ModelLoader loader("I:\\Development\\C++\\EmeraldOldStuff\\res\\sponza\\sponza.obj");
		for (auto& mesh : loader.load()) {
			Entity e = m_ecs.createEntity(mesh->getName());
			SceneGraphComponent* r = m_ecs.getComponent<SceneGraphComponent>(e);
			SceneGraphComponent* sponzaParent = m_ecs.getComponent<SceneGraphComponent>(sponza);
			m_ecs.addComponent<MeshRendererComponent>(e, mesh);
			sponzaParent->addChild(r);
		}
	}
	void Scene::load() {}
	void Scene::save() {}
	void Scene::update(Timestep ts) {}
	void Scene::getActiveCamera() {}
}