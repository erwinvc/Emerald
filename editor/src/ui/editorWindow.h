#pragma once
#include "emerald.h"
#include "logPanel.h"

namespace emerald {
	class EditorWindow {
	public:
		EditorWindow();
		~EditorWindow();

		void drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);
		void drawEditor(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);
		void drawViewport();
		void drawWindows();
		void update(Timestep ts);
		void fixedUpdate(Timestep ts);
		void onImGuiRender();
		void destroy();

		glm::ivec2 getSceneViewportSize() const;
		bool isViewportFocused() const;
		bool isMouseInViewport() const;

	private:
		Ref<Texture> m_icon;
		LogPanel m_logPanel;
	};
}