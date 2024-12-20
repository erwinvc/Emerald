#pragma once

namespace emerald {
	class ViewportPanel {
	public:
		ViewportPanel();
		void draw();
		void handleDragDrop();

		glm::ivec2 getSceneViewportSize() const { return m_sceneViewportSize;}
		bool isViewportFocused() const { return m_viewportFocused; }
		bool isMouseInViewport() const { return m_mouseInViewport; }

	private:
		bool m_mouseInViewport = false;
		bool m_viewportFocused = false;
		glm::vec2 m_sceneViewportSize = glm::vec2(1.0f, 1.0f);
	};
}