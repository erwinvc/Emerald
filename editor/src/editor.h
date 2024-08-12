#pragma once

namespace emerald {
	class RenderPipeline;

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
