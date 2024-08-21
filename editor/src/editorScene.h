#pragma once
#include "scene/scene.h"
#include "editorCamera.h"

namespace emerald {
	class EditorScene : public Scene {
	public:
		EditorScene(const std::filesystem::path& scenePath)
			: Scene(scenePath), editorCamera(Ref<EditorCamera>::create(70.0f, 0.001f, 1000.0f)) {
		}

		void load() override {
			//Scene::load();
		}

		void update(Timestep ts) override {
			editorCamera->update(ts);  
		}

	private:
		Ref<EditorCamera> editorCamera;
	};

}