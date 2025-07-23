#pragma once

namespace emerald {
	class ViewportPanel {
	public:
		ViewportPanel();
		void draw();
		void handleDragDrop();

		glm::ivec2 getSceneViewportSize() const { return m_sceneViewportSize; }
		glm::ivec2 getMousePositionInViewport() const { return m_mousePositionInViewport; }
		bool isViewportFocused() const { return m_viewportFocused; }
		bool isMouseInViewport() const { return m_mouseInViewport; }

	private:
		bool m_mouseInViewport = false;
		bool m_viewportFocused = false;
		glm::ivec2 m_sceneViewportSize = glm::ivec2(1.0f, 1.0f);
		glm::ivec2 m_mousePositionInViewport = glm::ivec2(0.0f, 0.0f);
	};
}