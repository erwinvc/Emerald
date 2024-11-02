#pragma once
#include "engine/scene/scene.h"
#include "inspectorTree.h"

namespace emerald {
	class HierarchyPanel;

	class InspectorPanel {
	public:
		void draw(Ref<Scene>& scene, HierarchyPanel* hierarchyPanel);

	private:
		float m_firstColumnWidth = 125.0f;
		float m_splitterWidth = 4.0f;
		InspectorTree m_inspectorTree;

		void drawInspectorHeader(Ref<Scene>& scene, std::vector<Entity>& selectedEntities);
		void drawSplitter(uint32_t totalWidth);
	};
}