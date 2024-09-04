#pragma once
#include "entities/components.h"

namespace emerald {
	class HierarchyTree {
	public:
		HierarchyTree();

		void render(Ref<Scene> scene);

	private:
		std::vector<SceneGraphComponent*> m_nodes;
		SceneGraphComponent* m_lastSelectedNode = nullptr;
		ImGuiSelectionBasicStorage m_imGuiSelection;

		void collectNodes(SceneGraphComponent* node);
		void renderNode(Scene* scene, SceneGraphComponent* node, int depth = 0);
		void drawInsertBeforeDropTarget();

		ImGuiTreeNodeFlags prepareTreeNodeFlags(SceneGraphComponent* node, bool isRootNode);
		ImVector<SceneGraphComponent*> getSelectedNodes();

		void addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* parent, bool insertBefore, SceneGraphComponent* beforeNode);
		bool isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node);

		void onDrag(SceneGraphComponent* node);
		void onDrop(SceneGraphComponent* node, bool insertBefore, SceneGraphComponent* beforeNode, bool open);
		void onRightClick(SceneGraphComponent* node);
	};
}
