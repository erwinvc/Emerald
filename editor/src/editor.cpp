#include "eepch.h"
#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"
#include "graphics/renderPipeline.h"
#include "editor.h"
#include "graphics/renderer.h"
#include "imguiProfiler/Profiler.h"

namespace emerald {
	std::unique_ptr<EditorWindow> editorWindow;
	std::unique_ptr<RenderPipeline> renderPipeline;

	void EmeraldEditorApplication::onInitialize() {
		editorWindow = std::make_unique<EditorWindow>();
		renderPipeline = std::make_unique<RenderPipeline>();
		editorWindow->initialize();

		gCPUProfiler.Initialize(8, 1024 * 4);
		gGPUProfiler.Initialize(8, 1024 * 4);
	}

	void EmeraldEditorApplication::onShutdown() {
		editorWindow.reset();
		renderPipeline.reset();
	}

	int index = 0;
	void EmeraldEditorApplication::update(Timestep ts) {
		editorWindow->update(ts);
		Renderer::submit([ts] {
			PROFILE_RENDER_BEGIN("ImGui start");
			imGuiManager::begin();
			PROFILE_RENDER_END();
			PROFILE_RENDER_BEGIN("ImGui render");
			editorWindow->onImGuiRender();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("ImGui end");
			imGuiManager::end();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("Renderer");
			renderPipeline->render();
			PROFILE_RENDER_END();
		});
		index++;
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