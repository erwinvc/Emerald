#include "eepch.h"
#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"
#include "graphics/renderPipeline.h"
#include "editor.h"
#include "graphics/renderer.h"

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
		editorWindow->update(ts);

		Renderer::submit([ts] {
			imGuiManager::begin();
			editorWindow->onImGuiRender();
			imGuiManager::end();
			renderPipeline->render();
		});

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