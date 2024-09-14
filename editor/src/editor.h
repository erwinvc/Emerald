#pragma once
#include "scene/scene.h"
#include "editorCamera.h"
#include "application.h"

namespace emerald {
	class RenderPipeline;

	struct EditorHeaderInformation {
		std::string title = "Emerald Engine Editor";
		std::string subTitle = "";
	};

	struct EditorWindowsVisibility {
		bool profiler = false;
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

		const Ref<Texture>& getFinalTexture();
		const Ref<EditorCamera>& getEditorCamera();
	};

	inline EmeraldEditorApplication* Editor;
}
