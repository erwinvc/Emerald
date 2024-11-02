#pragma once
#include "core/common/emerald.h"
#include "logPanel.h"
#include "inspectorPanel.h"
#include "hierarchyPanel.h"
#include "assetBrowserPanel.h"
#include "scenePanel.h"

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
		HierarchyPanel m_hierarchyPanel;
		InspectorPanel m_inspectorPanel;
		LogPanel m_logPanel;
		AssetBrowserPanel m_assetBrowserPanel;
		ScenePanel m_scenePanel;
	};
}