#pragma once
#include "emerald.h"

namespace emerald {
	class EditorWindow {
	public:
		EditorWindow() = default;
		~EditorWindow() = default;

		void drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);

		void initialize();
		void update(Timestep ts);
		void fixedUpdate(Timestep ts);
		void onImGuiRender();
		void destroy();
	};
}