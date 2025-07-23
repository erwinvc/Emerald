#pragma once
#include "core/common/emerald.h"
#include "logPanel.h"
#include "inspectorPanel.h"
#include "hierarchyPanel.h"
#include "assetBrowserPanel.h"
#include "scenePanel.h"
#include "viewportPanel.h"

namespace emerald {
	class EditorWindow {
	public:
		EditorWindow();
		~EditorWindow();

		void drawTitlebar(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);
		void drawEditor(ImVec2 pos, ImVec2 size, ImGuiID viewportID, float titlebarHeight);
		void drawWindows();
		void update(Timestep ts);
		void render();
		void fixedUpdate(Timestep ts);
		void onImGuiRender();

		glm::ivec2 getSceneViewportSize() const { return m_viewportPanel.getSceneViewportSize(); }
		glm::ivec2 getMousePositionInViewport() const { return m_viewportPanel.getMousePositionInViewport(); }
		bool isViewportFocused() const { return m_viewportPanel.isViewportFocused(); }
		bool isMouseInViewport() const { return m_viewportPanel.isMouseInViewport(); }

	private:
		Ref<Texture> m_icon;
		HierarchyPanel m_hierarchyPanel;
		InspectorPanel m_inspectorPanel;
		ViewportPanel m_viewportPanel;
		LogPanel m_logPanel;
		AssetBrowserPanel m_assetBrowserPanel;
		ScenePanel m_scenePanel;
	};
}