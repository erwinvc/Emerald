#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"

namespace emerald {
	class EmeraldEditorApplication : public Application {
	public:
		EditorWindow m_editorWindow;

		void onInitialize() override {
			m_editorWindow.initialize();
		}

		void onShutdown() override {
		}

		void update(Timestep ts) override {
			imGuiManager::begin();
			m_editorWindow.update(ts);
			imGuiManager::end();
		}

		void fixedUpdate(Timestep ts) override {
			m_editorWindow.fixedUpdate(ts);
		}
	};

	Application* createApplication(int argc, char** argv) {
		return new EmeraldEditorApplication();
	}
}