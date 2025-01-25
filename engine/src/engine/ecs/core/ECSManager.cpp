#include "eepch.h"
#include "ECSManager.h"
#include "entityComponentSystem.h"

namespace emerald {
	static EntityComponentSystem* s_activeECS;
	static EntityComponentSystem* s_editorECS;
	static EntityComponentSystem* s_playECS;

	void ECSManager::Initialize(bool isEditor) {
		if (!isEditor) {
			s_activeECS = new EntityComponentSystem();
		} else {
			s_editorECS = new EntityComponentSystem();
			s_playECS = new EntityComponentSystem();

			s_activeECS = s_editorECS;
		}
	}

	void ECSManager::Shutdown() {
		delete s_activeECS;
		delete s_editorECS;
		delete s_playECS;
	}

	void ECSManager::SetActiveECS(bool playMode) {
		if (playMode && s_playECS) {
			s_activeECS = s_playECS;
		} else if (!playMode && s_editorECS) {
			s_activeECS = s_editorECS;
		}
	}

	EntityComponentSystem& ECSManager::ECS() { return *s_activeECS; }
	EntityComponentSystem& ECSManager::EditorECS() { return *s_editorECS; }
	EntityComponentSystem& ECSManager::PlayECS() { return *s_playECS; }
}