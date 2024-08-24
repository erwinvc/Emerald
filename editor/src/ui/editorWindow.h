#pragma once
#include "emerald.h"

namespace emerald {
	class EditorWindow {
	public:
		EditorWindow();;
		~EditorWindow();

		void drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);

		void update(Timestep ts);
		void fixedUpdate(Timestep ts);
		void onImGuiRender();
		void destroy();

		glm::vec2 getSceneViewportSize() const;

	private:
		Ref<Texture> m_icon;
	};
}