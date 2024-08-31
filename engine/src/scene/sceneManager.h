#pragma once
#include "scene.h"

namespace emerald {
	class SceneManager {
	public:
		static void setActiveScene(Ref<Scene> scene) {
			m_activeScene = scene;
		}

		static Ref<Scene> getActiveScene() {
			return m_activeScene;
		}

		static void setRuntimeScene(Ref<Scene> scene) {
			m_runtimeScene = scene;
		}

		static void play() {
		}

		static void stop() {
		}

		static bool isPlaying() {
			return m_editorScene != nullptr;
		}

	private:
		static inline Ref<Scene> m_activeScene = nullptr; 
		static inline Ref<Scene> m_runtimeScene = nullptr;
		static inline Ref<Scene> m_editorScene = nullptr;
	};
}
