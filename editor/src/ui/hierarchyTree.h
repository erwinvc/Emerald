#pragma once
#include "entities/components.h"
#include "scene/scene.h"

namespace emerald {
	//struct HierarchyTreeNode {
	//	std::string name;
	//	std::vector<HierarchyTreeNode*> children;
	//	HierarchyTreeNode* parent;
	//
	//	HierarchyTreeNode(const std::string& n) : name(n), parent(nullptr) {}
	//};

	class HierarchyTree {
	public:
		void render(Ref<Scene> scene);

	private:
		SceneGraphComponent* selectedNode = nullptr;
		SceneGraphComponent* draggedNode = nullptr;
		SceneGraphComponent* dragTargetNode = nullptr;
		SceneGraphComponent* dragTargetParent = nullptr;
		bool isInsertingBefore = false;
		bool isDragging = false;

		void renderNode(Scene* scene, SceneGraphComponent* node, int depth);
		void removeNodeFromParent(SceneGraphComponent* node);
		void addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* newParent, bool insertBefore = false, SceneGraphComponent* beforeNode = nullptr);
		bool isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node);
	};
}
