#pragma once

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
		void onInitialize() override;
		void onShutdown() override;
		void update(Timestep ts) override;
		void fixedUpdate(Timestep ts) override;

		RenderPipeline* pipeline();
	};
	inline EmeraldEditorApplication* Editor;
}
