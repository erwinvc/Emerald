#pragma once
#include "core/application/application.h"
#include "editorCamera.h"
#include "../editor/events/editorProjectOpenedEvent.h"
#include "engine/ecs/core/entity.h"

namespace emerald {
	class RenderPipeline;

	struct EditorHeaderInformation {
		std::string title = "Emerald Engine Editor";
		std::string subTitle = "";
	};

	struct EditorWindowsVisibility {
		bool profiler = false;
		bool demo = false;
	};

	inline EditorWindowsVisibility EditorWindows;
	inline EditorHeaderInformation EditorHeader;

	class EmeraldEditorApplication : public Application {
	public:
		EmeraldEditorApplication();

		void onInitialize() override;
		void onShutdown() override;
		void update(Timestep ts) override;
		void fixedUpdate(Timestep ts) override;
		void onEvent(Event& event) override;

		const Ref<Texture>& getFinalTexture();
		const Ref<EditorCamera>& getEditorCamera();

	private:

		void onProjectOpened(EditorProjectOpenedEvent& e);
	};

	inline EmeraldEditorApplication* Editor;
}
