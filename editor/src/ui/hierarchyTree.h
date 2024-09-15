#pragma once
#include "entities/components.h"
#include "scene/scene.h"
#include "ui/imguiManager.h"
#include "entities/entity.h"

namespace emerald {
	class HierarchyTree {
	public:
		HierarchyTree();

		void render(const Ref<Scene>& scene, const char* searchString);
		void handleDelete();
		std::vector<Entity>& getSelectedEntities();

	private:
		bool m_isFocused = false;
		std::vector<Entity> m_selectedEntities;
		std::vector<SceneGraphComponent*> m_nodes;
		SceneGraphComponent* m_lastSelectedNode = nullptr;
		ImGuiSelectionBasicStorage m_imGuiSelection;

		void collectNodes(SceneGraphComponent* node);
		void renderNode(Scene* scene, SceneGraphComponent* node, const char* searchString, int depth = 0);
		void drawInsertBeforeDropTarget();

		ImGuiTreeNodeFlags prepareTreeNodeFlags(SceneGraphComponent* node, bool isRootNode);
		ImVector<SceneGraphComponent*> getSelectedNodes();

		static void addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* parent, bool insertBefore, SceneGraphComponent* beforeNode);
		static bool isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node);

		void onDrag(SceneGraphComponent* node);
		void onDrop(SceneGraphComponent* node, bool insertBefore, SceneGraphComponent* beforeNode, bool open);
		void onRightClick(SceneGraphComponent* node);
	};
}
