#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"
#include "graphics/renderPipeline.h"
#include "editor.h"

namespace emerald {
	std::unique_ptr<EditorWindow> editorWindow;
	std::unique_ptr<RenderPipeline> renderPipeline;

	void EmeraldEditorApplication::onInitialize() {
		editorWindow = std::make_unique<EditorWindow>();
		renderPipeline = std::make_unique<RenderPipeline>();
		editorWindow->initialize();
	}

	void EmeraldEditorApplication::onShutdown() {
		editorWindow.reset();
		renderPipeline.reset();
	}

	void EmeraldEditorApplication::update(Timestep ts) {

		imGuiManager::begin();
		editorWindow->update(ts);
		imGuiManager::end();

		renderPipeline->render();

	}

	void EmeraldEditorApplication::fixedUpdate(Timestep ts) {
		editorWindow->fixedUpdate(ts);
	}

	RenderPipeline* EmeraldEditorApplication::pipeline() {
		return renderPipeline.get();
	}

	Application* createApplication(int argc, char** argv) {
		return new EmeraldEditorApplication();
	}
}