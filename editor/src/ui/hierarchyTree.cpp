#include "eepch.h"
#include "editor.h"
#include "entities/componentArray.h"
#include "hierarchyTree.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "ui/imguiManager.h"

namespace emerald {
	void HierarchyTree::renderNode(Scene* scene, SceneGraphComponent* node, int depth) {
		NameComponent& nameComponent = scene->getECS().getComponent<NameComponent>(node->m_entity);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere;

		if (node->m_children.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (selectedNode == node) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		flags |= ImGuiTreeNodeFlags_FramePadding;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
		ImGui::InvisibleButton(("##insert_before_" + nameComponent.m_name).c_str(), ImVec2(-1, 4), ImGuiButtonFlags_AllowOverlap);
		ImGui::PopItemFlag();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TREE_NODE")) {
				SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;
				dragTargetNode = node;
				dragTargetParent = node->m_parent;
				isInsertingBefore = true;
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SetNextItemOpen(node->m_isOpenInHierarchy);
		bool wasOpen = node->m_isOpenInHierarchy = ImGui::TreeNodeEx((void*)(intptr_t)node, flags, nameComponent.m_name.c_str());

		if (ImGui::IsItemFocused()) {
			selectedNode = node;
		}

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			draggedNode = node;
			isDragging = true;
		}

		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("TREE_NODE", &node, sizeof(SceneGraphComponent*));
			//ImGui::Text("Dragging %s", node->name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TREE_NODE")) {
				SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;
				dragTargetNode = node;
				dragTargetParent = node;
				isInsertingBefore = false;

				node->m_isOpenInHierarchy = true;
			}
			ImGui::EndDragDropTarget();
		}

		if (wasOpen) {
			for (auto& child : node->m_children) {
				renderNode(scene, child, depth + 1);
			}
			ImGui::TreePop();
		}
	}

	void HierarchyTree::removeNodeFromParent(SceneGraphComponent* node) {
		if (node->m_parent) {
			node->m_parent->removeChild(node);
			node->m_parent = 0;
		}
	}

	void HierarchyTree::addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* newParent, bool insertBefore, SceneGraphComponent* beforeNode) {
		if (newParent) {
			if (insertBefore && beforeNode) {
				auto it = std::find(newParent->m_children.begin(), newParent->m_children.end(), beforeNode);
				if (it != newParent->m_children.end()) {
					newParent->m_children.insert(it, node);
				} else {
					newParent->m_children.push_back(node);
				}
			} else {
				newParent->m_children.push_back(node);
			}
		}

		node->m_parent = newParent;
	}

	bool HierarchyTree::isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node) {
		while (node != nullptr) {
			if (node == possibleParent)
				return true;
			node = node->m_parent;
		}
		return false;
	}

	void HierarchyTree::render(Ref<Scene> scene) {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::ItemRowsBackground(18);

		for (auto& node : scene->getECS().getComponentArray<SceneGraphComponent>()->getVector()) {
			if (node->m_parent == nullptr) {
				renderNode(scene.raw(), node.raw(), 0);
			}
		}

		// Handle dragging outside of any node (unparenting)
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("TREE_NODE")) {
				SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;
				dragTargetNode = nullptr;
				dragTargetParent = nullptr;
				isInsertingBefore = false;
			}
			ImGui::EndDragDropTarget();
		}

		// Process drag and drop
		if (isDragging && !ImGui::IsMouseDown(0)) {
			isDragging = false;
			if (draggedNode && dragTargetNode && dragTargetNode != draggedNode) {
				if (!isAncestor(draggedNode, dragTargetNode)) {
					removeNodeFromParent(draggedNode);
					addNodeToParent(draggedNode, dragTargetParent, isInsertingBefore, dragTargetNode);
				}
			}
			draggedNode = nullptr;
			dragTargetNode = nullptr;
			dragTargetParent = nullptr;
		}
		ImGui::PopStyleVar(2);
	}
}