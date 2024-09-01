#include "eepch.h"
#include "scene.h"
#include "entities/components.h"
#include "assets/loaders/modelLoader.h"

namespace emerald {
	static std::vector<Ref<Mesh>> meshes;
	Scene::Scene(const std::filesystem::path& scenePath) : m_path(scenePath) {

	}

	void Scene::initialize() {
		m_ecs.registerComponent<SceneGraphComponent>();
		m_ecs.registerComponent<TransformComponent>();
		m_ecs.registerComponent<NameComponent>();
		m_ecs.registerComponent<UUIDComponent>();

		TransformComponent t;
		
		Entity sponza = Entity::create("sponza");

		Entity test1 = Entity::create("test1");
		Entity test2 = Entity::create("test2");
		Entity test3 = Entity::create("test3");
		Entity test4 = Entity::create("test4");
		Entity test5 = Entity::create("test5");
		//test1.getComponent<SceneGraphComponent>().setChild(test2);
		//test2.getComponent<SceneGraphComponent>().setChild(test3);
		//test3.getComponent<SceneGraphComponent>().setChild(test4);
		//test3.getComponent<SceneGraphComponent>().setChild(test5);

		ModelLoader loader("I:\\Development\\C++\\EmeraldOldStuff\\res\\sponza\\sponza.obj");
		for (auto& mesh : loader.load()) {
			Entity e = m_ecs.createEntity(mesh->getName());
			SceneGraphComponent& r = m_ecs.getComponent<SceneGraphComponent>(e);
			SceneGraphComponent& sponzaParent = m_ecs.getComponent<SceneGraphComponent>(sponza);
			auto  a = sponzaParent.getParentClassType();
			auto  b = SceneGraphComponent::getParentClassType();
			sponzaParent.addChild(&r);
		}
	}
	void Scene::load() {}
	void Scene::save() {}
	void Scene::update(Timestep ts) {}
	void Scene::getActiveCamera() {}
}