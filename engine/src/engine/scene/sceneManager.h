#pragma once
#include "scene.h"

namespace emerald {
	class SceneManager {
	public:
		static void setActiveScene(const Ref<Scene>& scene) {
			m_activeScene = scene;
		}

		static Ref<Scene>& getActiveScene() {
			return m_activeScene;
		}

		static void setRuntimeScene(const Ref<Scene>& scene) {
			m_runtimeScene = scene;
		}

		static bool isSceneOpen() {
			return m_activeScene != nullptr;
		}

		static void play() {
		}

		static void stop() {

		}

		static bool isPlaying() {
			return m_editorScene != nullptr;
		}

		static void clearScenes() {
			m_activeScene = nullptr;
			m_runtimeScene = nullptr;
			m_editorScene = nullptr;
		}

	private:
		static inline Ref<Scene> m_activeScene = nullptr; 
		static inline Ref<Scene> m_runtimeScene = nullptr;
		static inline Ref<Scene> m_editorScene = nullptr;
	};
}
