#pragma once
#include "engine/ecs/components/sceneGraphComponent.h"
#include "engine/ecs/core/entity.h"
#include "engine/scene/scene.h"
#include "ui/imguiManager.h"

namespace emerald {
	class HierarchyTree {
	public:
		HierarchyTree();

		void draw(const Ref<Scene>& scene, const char* searchString);
		void handleDelete();
		//std::vector<Entity>& getSelectedEntities();

	private:
		bool m_isFocused = false;
		std::vector<SceneGraphComponent*> m_nodes;
		SceneGraphComponent* m_lastSelectedNode = nullptr;
		ImGuiSelectionBasicStorage m_imGuiSelection;
		std::vector<Entity> m_selectedEntities;

		void collectNodes(SceneGraphComponent* node);
		ImRect renderNode(SceneGraphComponent* node, const char* searchString, int depth = 0);
		void drawInsertBeforeDropTarget();

		ImGuiTreeNodeFlags prepareTreeNodeFlags(SceneGraphComponent* node, bool isRootNode);
		ImVector<SceneGraphComponent*> getSelectedNodes();

		static void addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* parent, bool insertBefore, SceneGraphComponent* beforeNode);
		static bool isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node);

		void onDrag(SceneGraphComponent* node);
		bool onDrop(SceneGraphComponent* node, bool insertBefore, SceneGraphComponent* beforeNode, bool open);
		void onRightClick(SceneGraphComponent* node);

		void updateSelectedEntities();
	};
}
