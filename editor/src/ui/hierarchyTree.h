#pragma once
#include "entities/components.h"
#include "scene/scene.h"
#include "input/mouse.h"

namespace emerald {
	class HierarchyTree {
	public:
		void render(Ref<Scene> scene);

	private:
		std::vector<SceneGraphComponent*> selectedNodes;
		SceneGraphComponent* lastSelectedNode = nullptr;

		void renderNode(Scene* scene, SceneGraphComponent* node, int depth);
		void addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* parent, bool insertBefore, SceneGraphComponent* beforeNode);
		bool isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node);

		//Interactions
		void onDrag(SceneGraphComponent* node);
		void onDrop(SceneGraphComponent* node);
		void onLeftClick(SceneGraphComponent* node);
		void onRightClick(SceneGraphComponent* node);
		void handleClickSelection(SceneGraphComponent* node, MouseButton button, bool openPopup);

		//Selection
		void addSelectedNode(SceneGraphComponent* node, bool sort);
		void removeSelectedNode(SceneGraphComponent* node, bool sort);
		bool isNodeSelected(SceneGraphComponent* node);
		void selectRange(SceneGraphComponent* startNode, SceneGraphComponent* endNode);
		void sortSelectedNodes();
	};
}
