#pragma once
#include "inspectorTree.h"
#include "utils\datastructures\vector.h"

namespace emerald {
	class HierarchyPanel;

	class InspectorPanel {
	public:
		void draw(HierarchyPanel* hierarchyPanel);

	private:
		float m_firstColumnWidth = 125.0f;
		float m_splitterWidth = 4.0f;
		InspectorTree m_inspectorTree;

		void drawInspectorHeader(const Vector<Entity>& selectedEntities);
		void drawSplitter(uint32_t totalWidth);
	};
}