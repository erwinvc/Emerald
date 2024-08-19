#include "eepch.h"
#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"
#include "graphics/renderPipeline.h"
#include "editor.h"
#include "graphics/renderer.h"
#include "imguiProfiler/Profiler.h"
#include "project.h"

namespace emerald {
	static std::unique_ptr<EditorWindow> editorWindow;
	static std::unique_ptr<RenderPipeline> renderPipeline;
	static float s_lastTitleUpdateTime = 0.0f;

	void updateTitlebar(float time, uint32_t ups, uint32_t fps, bool force = false) {
		if (time - s_lastTitleUpdateTime >= 1.0f || force) {
			EditorHeader.subTitle = std::format("UPS: {} FPS: {}", ups, fps);
			s_lastTitleUpdateTime = time;
		}
	}

	void EmeraldEditorApplication::onInitialize() {
		editorWindow = std::make_unique<EditorWindow>();
		renderPipeline = std::make_unique<RenderPipeline>();
		editorWindow->initialize();

		PROFILE_INITIALIZE();
		PROFILE_DISABLE();

		updateTitlebar(0, 0, 0, true);
	}

	void EmeraldEditorApplication::onShutdown() {
		editorWindow.reset();
		renderPipeline.reset();
	}

	void EmeraldEditorApplication::update(Timestep ts) {
		PROFILE_LOGIC_BEGIN("Editor window update");
		updateTitlebar(getTime(), getUPS(), getFPS());
		editorWindow->update(ts);
		PROFILE_LOGIC_END();

		Renderer::submit([ts] {
			PROFILE_RENDER_BEGIN("ImGui start");
			ImGuiManager::begin();
			PROFILE_RENDER_END();
			PROFILE_RENDER_BEGIN("ImGui render");
			editorWindow->onImGuiRender();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("ImGui end");
			ImGuiManager::end();
			PROFILE_RENDER_END();

		});
		PROFILE_LOGIC_BEGIN("Pipeline render");
		renderPipeline->render();
		PROFILE_LOGIC_END();
	}

	void EmeraldEditorApplication::fixedUpdate(Timestep ts) {
		editorWindow->fixedUpdate(ts);
	}

	RenderPipeline* EmeraldEditorApplication::pipeline() {
		return renderPipeline.get();
	}

	Application* createApplication() {
		return new EmeraldEditorApplication();
	}
}