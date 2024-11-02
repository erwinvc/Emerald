#pragma once
#include "core/application/application.h"
#include "engine/scene/scene.h"
#include "editorCamera.h"

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
	};

	inline EmeraldEditorApplication* Editor;
}
