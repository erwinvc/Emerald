#include "eepch.h"
#include "graphics/core/camera.h"
#include "editor.h"
#include "engine/input/keyboard.h"
#include "engine/input/mouse.h"
#include "engine/scene/sceneManager.h"
#include "graphics/buffers/framebuffer.h"
#include "graphics/render/renderPipeline.h"
#include "imguiProfiler/Profiler.h"
#include "ui/editorWindow.h"
#include "undoRedo.h"
#include "graphics/core/renderer.h"
#include "core/common/engineError.h"
#include "core/common/engineLoading.h"
#include "engine/events/eventSystem.h"
#include "editorProjectOpenedEvent.h"
#include "engine/assets/core/assetRegistry.h"
#include "project.h"
#include "input/dragDrop.h"

namespace emerald {
	static UniqueRef<EditorWindow> s_editorWindow;
	static UniqueRef<RenderPipeline> s_renderPipeline;
	static Ref<EditorCamera> s_editorCamera;

	static float s_lastTitleUpdateTime = 0.0f;

	void updateTitlebar(float time, uint32_t ups, uint32_t fps, bool force = false) {
		if (time - s_lastTitleUpdateTime >= 1.0f || force) {
			EditorHeader.subTitle = std::format("UPS: {} FPS: {}", ups, fps);
			s_lastTitleUpdateTime = time;
		}
	}

	EmeraldEditorApplication::EmeraldEditorApplication() {
		Editor = this;

		PROFILE_INITIALIZE();
		PROFILE_DISABLE();
	}

	void EmeraldEditorApplication::onInitialize() {
		s_editorWindow = UniqueRef<EditorWindow>::create();
		s_renderPipeline = UniqueRef<RenderPipeline>::create();
		s_editorCamera = Ref<EditorCamera>::create(70.0f, 0.05f, 500.0f);

		Project::loadRecentProjects();

		updateTitlebar(0, 0, 0, true);
	}

	void EmeraldEditorApplication::onShutdown() {
		SceneManager::clearScenes();
		s_editorWindow.reset();
		s_renderPipeline.reset();
	}

	bool mouseActiveInViewport = false;
	void EmeraldEditorApplication::update(Timestep ts) {
		PROFILE_LOGIC_BEGIN("Editor window update");
		updateTitlebar(getTime(), getUPS(), getFPS());
		s_editorWindow->update(ts);
		PROFILE_LOGIC_END();

		//Undo redo

		if (Keyboard::keyJustDown(Key::H)) {
			Ref<Shader>::create("Geometry", "res/shaders/geometry");
			Renderer::submitBufferForRendering();
			Renderer::waitForBufferAvailability();
		}

		if (Keyboard::keyMod(KeyMod::CONTROL)) {
			if (Keyboard::keyJustDown(Key::Z) || Keyboard::keyRepeat(Key::Z)) {
				if (Keyboard::keyMod(KeyMod::SHIFT)) {
					UndoRedo::redo();
				} else UndoRedo::undo();
			}
		}

		glm::ivec2 viewportSize = s_editorWindow->getSceneViewportSize();
		s_editorCamera->setViewportSize(viewportSize.x, viewportSize.y);

		//Camera magic, this can probably be improved
		bool mouseDown = Mouse::buttonDown(MouseButton::LEFT) || Mouse::buttonDown(MouseButton::RIGHT);
		if (s_editorWindow->isViewportFocused()) {
			if (s_editorWindow->isMouseInViewport()) {
				if (mouseDown) {
					mouseActiveInViewport = true;
				}
			} else if (!mouseDown) mouseActiveInViewport = false;
		} else mouseActiveInViewport = false;
		if (mouseActiveInViewport) s_editorCamera->update(ts);

		Renderer::submit([viewportSize] {
			FrameBufferManager::onResize(viewportSize.x, viewportSize.y);
		});

		if (SceneManager::getActiveScene()) {
			SceneManager::getActiveScene()->update(ts);

			PROFILE_LOGIC_BEGIN("Pipeline render");
			s_renderPipeline->render();
			PROFILE_LOGIC_END();
		}

		Renderer::submit([ts, viewportSize] {
			PROFILE_RENDER_BEGIN("ImGui start");
			ImGuiManager::begin();
			PROFILE_RENDER_END();
			PROFILE_RENDER_BEGIN("ImGui render");
			s_editorWindow->onImGuiRender();
			EngineError::draw();
			EngineLoading::draw();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("ImGui end");
			DragDrop::handleDragDropVisuals();
			ImGuiManager::end();
			PROFILE_RENDER_END();
		});
	}

	void EmeraldEditorApplication::fixedUpdate(Timestep ts) {
		s_editorWindow->fixedUpdate(ts);
	}

	void EmeraldEditorApplication::onProjectOpened(EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			AssetRegistry::parseCurrentProject();
			SceneManager::setActiveScene(Ref<Scene>::create("New Scene", ""));
			SceneManager::getActiveScene()->initialize();
		} else {
			AssetRegistry::clear();
			SceneManager::clearScenes();
		}
	}

	void EmeraldEditorApplication::onEvent(Event& e) {
		EventSystem::routeEvent<EditorProjectOpenedEvent>(e, this, &EmeraldEditorApplication::onProjectOpened);
		//EventSystem::routeEvent<ErrorEvent>(e, [](ErrorEvent& e) { Log::error("{}", e.getMessage()); });
	}

	const Ref<Texture>& EmeraldEditorApplication::getFinalTexture() {
		return s_renderPipeline->getFinalTexture();
	}

	const Ref<EditorCamera>& EmeraldEditorApplication::getEditorCamera() {
		return s_editorCamera;
	}

	Application* createApplication() {
		return new EmeraldEditorApplication();
	}
}