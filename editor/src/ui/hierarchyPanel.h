#pragma once
#include "hierarchyTree.h"

namespace emerald {
	class HierarchyPanel {
	public:
		void draw();
		void update(Timestep ts);
	private:
		HierarchyTree m_hierarchyTree;
	};
}