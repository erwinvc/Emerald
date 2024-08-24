#include "eepch.h"
#include <windows.h>
#include "emerald.h"
#include "ui/editorWindow.h"
#include "graphics/renderPipeline.h"
#include "editor.h"
#include "graphics/renderer.h"
#include "imguiProfiler/Profiler.h"
#include "project.h"
#include "input/keyboard.h"
#include "editorScene.h"
#include "graphics/framebuffer.h"

namespace emerald {
	static UniqueRef<EditorWindow> s_editorWindow;
	static UniqueRef<RenderPipeline> s_renderPipeline;
	static Ref<EditorCamera> s_editorCamera;
	static Ref<Scene> s_activeScene;

	static float s_lastTitleUpdateTime = 0.0f;

	void updateTitlebar(float time, uint32_t ups, uint32_t fps, bool force = false) {
		if (time - s_lastTitleUpdateTime >= 1.0f || force) {
			EditorHeader.subTitle = std::format("UPS: {} FPS: {}", ups, fps);
			s_lastTitleUpdateTime = time;
		}
	}

	EmeraldEditorApplication::EmeraldEditorApplication() {
		PROFILE_INITIALIZE();
		PROFILE_DISABLE();
	}

	void EmeraldEditorApplication::onInitialize() {
		s_editorWindow = UniqueRef<EditorWindow>::create();
		s_renderPipeline = UniqueRef<RenderPipeline>::create();
		s_editorCamera = Ref<EditorCamera>::create(70.0f, 0.001f, 1000.0f);

		updateTitlebar(0, 0, 0, true);
	}

	void EmeraldEditorApplication::onShutdown() {
		s_editorWindow.reset();
		s_renderPipeline.reset();
	}

	void EmeraldEditorApplication::update(Timestep ts) {
		PROFILE_LOGIC_BEGIN("Editor window update");
		updateTitlebar(getTime(), getUPS(), getFPS());
		s_editorWindow->update(ts);
		PROFILE_LOGIC_END();

		glm::vec2 viewportSize = s_editorWindow->getSceneViewportSize();
		s_editorCamera->setViewportSize(viewportSize.x, viewportSize.y);
		s_editorCamera->update(ts);

		Renderer::submit([viewportSize] {
			FrameBufferManager::onResize(viewportSize.x, viewportSize.y);
		});

		PROFILE_LOGIC_BEGIN("Pipeline render");
		s_renderPipeline->render();
		PROFILE_LOGIC_END();


		if (s_activeScene) {
			s_activeScene->update(ts);
		}

		Renderer::submit([ts, viewportSize] {
			PROFILE_RENDER_BEGIN("ImGui start");
			ImGuiManager::begin();
			PROFILE_RENDER_END();
			PROFILE_RENDER_BEGIN("ImGui render");
			s_editorWindow->onImGuiRender();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("ImGui end");
			ImGuiManager::end();
			PROFILE_RENDER_END();
		});
	}

	void EmeraldEditorApplication::fixedUpdate(Timestep ts) {
		s_editorWindow->fixedUpdate(ts);
	}

	Ref<Texture> EmeraldEditorApplication::getFinalTexture() {
		return s_renderPipeline->getFinalTexture();
	}

	Ref<Scene> EmeraldEditorApplication::getActiveScene() {
		return s_activeScene;
	}

	Ref<EditorCamera> EmeraldEditorApplication::getEditorCamera() {
		return s_editorCamera;
	}

	Application* createApplication() {
		return new EmeraldEditorApplication();
	}
}