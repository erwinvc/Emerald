#include "eepch.h"
#include "util/color.h"
#include "editor.h"
#include "hierarchyTree.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "scene/sceneManager.h"
#include "ui/imguiManager.h"
#include "ui/iconsFontAwesome.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "util/utils.h"
#include "undoRedo.h"
#include "ecs/components/nameComponent.h"

namespace emerald {
	HierarchyTree::HierarchyTree() {
		m_imGuiSelection.PreserveOrder = false;
		m_imGuiSelection.UserData = (void*)&m_nodes;
		m_imGuiSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			auto& nodes = *(std::vector<SceneGraphComponent*>*)self->UserData;
			return nodes[idx]->m_id;
		};
	}

	void HierarchyTree::render(const Ref<Scene>& scene, const char* searchString) {
		// Apply ImGui style settings
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::ItemRowsBackground(18, IM_COL32(28, 28, 28, 255));

		// Collect nodes
		m_nodes.clear();
		collectNodes(scene->getRootNode());

		// Begin multi-selection and render nodes
		auto* multiSelectIO = ImGui::BeginMultiSelect(ImGuiMultiSelectFlags_None, (int)m_imGuiSelection.Size, (int)m_nodes.size());
		m_imGuiSelection.ApplyRequests(multiSelectIO);
		renderNode(scene.raw(), scene->getRootNode(), searchString);
		onDrop(scene->getRootNode(), true, scene->getRootNode(), true);
		multiSelectIO = ImGui::EndMultiSelect();
		m_imGuiSelection.ApplyRequests(multiSelectIO);

		// Restore ImGui style settings
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();

		m_isFocused = ImGui::IsWindowFocused();
	}

	void HierarchyTree::handleDelete() {
		if (m_isFocused && Keyboard::keyJustDown(Key::DEL)) {
			auto selectedNodes = getSelectedNodes();
			auto action = UndoRedo::createAction<void>("Delete entities");

			// Capture the state of the entities before deleting them
			//for (auto& node : selectedNodes) {
			//	if (!node) continue;
			//
			//	// Get the entity and its components
			//	uint32_t entityID = node->m_entity;
			//	SceneGraphComponent* parent = node->m_parent;
			//	auto originalPosition = utils::getIndexInVector(parent->m_children, node);
			//	std::vector<Ref<Component>> components = SceneManager::getActiveScene()->getECS().getAllComponentsForEntity(entityID);
			//
			//	// Undo: Restore the entity
			//	action->addUndoAction([entityID, parentEntity = parent->m_entity, originalPosition, components]() {
			//		Ref<Scene> scene = SceneManager::getActiveScene();
			//		Entity restoredEntity = scene->getECS().createEntityFromID(entityID);  // Restore entity with original ID
			//		SceneGraphComponent* parentNode = parentEntity == 0 ? scene->getRootNode() : scene->getECS().getComponent<SceneGraphComponent>(parentEntity);
			//		SceneGraphComponent* restoredNode = scene->getECS().getComponent<SceneGraphComponent>(restoredEntity);
			//
			//		// Restore entity's position in the hierarchy
			//		restoredNode->setParent(nullptr);
			//		parentNode->m_children.insert(parentNode->m_children.begin() + originalPosition, restoredNode);
			//		restoredNode->m_parent = parentNode;
			//
			//		// Restore components
			//		for (auto& component : components) {
			//			scene->getECS().addComponent(restoredEntity, component);
			//		}
			//	});
			//
			//	// Redo: Destroy the entity
			//	action->addDoAction([entityID]() {
			//		SceneManager::getActiveScene()->getECS().destroyEntity(entityID);
			//	});
			//
			//	// Perform the initial delete
			//	SceneManager::getActiveScene()->getECS().destroyEntity(entityID);
			//}

			// Commit the action to the undo/redo stack
			UndoRedo::commitAction(action);
		}
	}

	void HierarchyTree::collectNodes(SceneGraphComponent* node) {
		uint32_t index = 0;

		static std::function<void(SceneGraphComponent*)> _collectNodes = [&](SceneGraphComponent* node) {
			m_nodes.push_back(node);
			node->m_treeIndex = index++;
			node->m_id = ImGui::GetID((void*)(intptr_t)node);
			for (auto& child : node->m_children) {
				_collectNodes(child);
			}
		};

		_collectNodes(node);
	}

	void HierarchyTree::renderNode(Scene* scene, SceneGraphComponent* node, const char* searchString, int depth) {
		if (!node) return;
		bool isRootNode = depth == 0;
		NameComponent* nameComponent = scene->getECS().getComponent<NameComponent>(node->m_entity);

		if (!isRootNode && ImGui::IsDragDropActive()) {
			drawInsertBeforeDropTarget();
			onDrop(node->m_parent, true, node, false);
		}

		ImGuiTreeNodeFlags flags = prepareTreeNodeFlags(node, isRootNode);
		ImGui::SetNextItemOpen(node->m_isOpenInHierarchy || isRootNode);

		if (isRootNode) {
			ImGui::Indent();
			ImGui::PushStyleColor(ImGuiCol_Header, Color(0.07f, 0.07f, 0.07f, 1.0f));
		}

		if (!ImGui::IsDragDropActive()) {
			ImGui::SetNextItemSelectionUserData(utils::getIndexInVector(m_nodes, node));
		}

		node->m_isOpenInHierarchy = ImGui::TreeNodeEx((void*)(intptr_t)node->m_id, flags, nameComponent->m_name.c_str());

		if (ImGui::IsItemFocused()) {
			m_lastSelectedNode = node;
		}

		if (isRootNode) {
			ImGui::PopStyleColor();
		}

		if (!isRootNode) {
			onDrag(node);
			onDrop(node, false, node, true);
			onRightClick(node);
		}

		if (node->m_isOpenInHierarchy) {
			for (auto& child : node->m_children) {
				renderNode(scene, child, searchString, depth + 1);
			}
			ImGui::TreePop();
		}
	}

	void HierarchyTree::drawInsertBeforeDropTarget() {
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
		ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
		ImGui::InvisibleButton("##insert_before_", ImVec2(-1, 4), ImGuiButtonFlags_AllowOverlap);
		ImGui::PopItemFlag();
		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
	}

	ImGuiTreeNodeFlags HierarchyTree::prepareTreeNodeFlags(SceneGraphComponent* node, bool isRootNode) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_FramePadding;

		if (isRootNode) {
			flags |= ImGuiTreeNodeFlags_Framed;
		}

		if (node->m_children.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}

		if (!isRootNode && m_imGuiSelection.Contains(node->m_id)) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		return flags;
	}

	ImVector<SceneGraphComponent*> HierarchyTree::getSelectedNodes() {
		ImVector<SceneGraphComponent*> selectedNodes;
		void* iterator = nullptr;
		ImGuiID id = 0;

		while (m_imGuiSelection.GetNextSelectedItem(&iterator, &id)) {
			auto foundNode = std::find_if(m_nodes.begin(), m_nodes.end(), [id](SceneGraphComponent* n) {
				return n->m_id == id;
			});

			if (foundNode != m_nodes.end()) {
				selectedNodes.push_back(*foundNode);
			}
		}

		std::sort(selectedNodes.begin(), selectedNodes.end(), [](SceneGraphComponent* a, SceneGraphComponent* b) {
			return a->m_treeIndex < b->m_treeIndex;
		});

		return selectedNodes;
	}

	void HierarchyTree::onDrag(SceneGraphComponent* node) {
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("NodeDragDrop", &node, sizeof(SceneGraphComponent*), ImGuiCond_Once);

			std::string name;
			ImVector<SceneGraphComponent*> selectedNodes = getSelectedNodes();
			for (int i = 0; i < selectedNodes.size(); i++) {
				auto* nameComponent = SceneManager::getActiveScene()->getECS().getComponent<NameComponent>(selectedNodes[i]->m_entity);
				name += nameComponent->m_name;
				if (i < selectedNodes.size() - 1) name += "\n";
			}

			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}
	}

	void HierarchyTree::onDrop(SceneGraphComponent* node, bool insertBefore, SceneGraphComponent* beforeNode, bool open) {
		if (!node) return;
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
				ImVector<SceneGraphComponent*> selectedNodes = getSelectedNodes();

				auto action = UndoRedo::createAction<void>("Move nodes");

				for (auto& droppedNode : selectedNodes) {
					if (droppedNode && node != droppedNode) {
						if (!isAncestor(droppedNode, node)) {
							SceneGraphComponent* originalParent = droppedNode->m_parent;
							auto originalPosition = utils::getIndexInVector(originalParent->m_children, droppedNode);

							SceneGraphComponent* originalPrevSibling = originalPosition > 0 ? originalParent->m_children[originalPosition - 1] : nullptr;
							SceneGraphComponent* originalNextSibling = (originalPosition < originalParent->m_children.size() - 1) ? originalParent->m_children[originalPosition + 1] : nullptr;

							addNodeToParent(droppedNode, node, insertBefore, beforeNode);

							UUID beforeNodeEntity = beforeNode ? beforeNode->m_entity : Entity();

							action->addUndoAction([droppedNodeEntity = droppedNode->m_entity, originalParentEntity = originalParent->m_entity, originalPrevSibling, originalNextSibling]() {
								SceneGraphComponent* droppedNode1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(droppedNodeEntity);
								SceneGraphComponent* originalParent1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(originalParentEntity);

								droppedNode1->setParent(nullptr);

								if (originalPrevSibling) {
									auto it = std::find(originalParent1->m_children.begin(), originalParent1->m_children.end(), originalPrevSibling);
									originalParent1->m_children.insert(it + 1, droppedNode1); //insert after the previous sibling
								} else if (originalNextSibling) {
									auto it = std::find(originalParent1->m_children.begin(), originalParent1->m_children.end(), originalNextSibling);
									originalParent1->m_children.insert(it, droppedNode1); //insert before the next sibling
								} else {
									originalParent1->m_children.push_back(droppedNode1); //insert at the end if no siblings
								}
								droppedNode1->m_parent = originalParent1;
							});

							action->addDoAction([droppedNodeEntity = droppedNode->m_entity, nodeEntity = node->m_entity, insertBefore, beforeNodeEntity]() {
								SceneGraphComponent* droppedNode1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(droppedNodeEntity);
								SceneGraphComponent* node1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(nodeEntity);
								SceneGraphComponent* beforeNode1 = beforeNodeEntity != 0 ? SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(beforeNodeEntity) : nullptr;
								addNodeToParent(droppedNode1, node1, insertBefore, beforeNode1);
							});
						}
					}
				}

				UndoRedo::commitAction(action);

				if (open && node) node->m_isOpenInHierarchy = true;
			}
			ImGui::EndDragDropTarget();
		}
	}

	void HierarchyTree::onRightClick(SceneGraphComponent* node) {
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
			ImGui::OpenPopup("HierarchyNodeOptions");

		ImGui::PushEmeraldPopupStyle();
		if (ImGui::BeginPopup("HierarchyNodeOptions")) {
			if (ImGui::Selectable("Cut")) { /* Handle cut */ }
			if (ImGui::Selectable("Copy")) { /* Handle copy */ }
			if (ImGui::Selectable("Paste")) { /* Handle paste */ }
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 1));
			if (ImGui::Selectable("Rename")) { /* Handle rename */ }
			if (ImGui::Selectable("Duplicate")) { /* Handle duplicate */ }
			if (ImGui::Selectable("Delete")) { /* Handle delete */ }
			ImGui::EndPopup();
		}
		ImGui::PopEmeraldPopupStyle();
	}

	void HierarchyTree::addNodeToParent(SceneGraphComponent* node, SceneGraphComponent* parent, bool insertBefore, SceneGraphComponent* beforeNode) {
		node->setParent(nullptr);

		if (parent) {
			if (insertBefore && beforeNode) {
				auto it = std::find(parent->m_children.begin(), parent->m_children.end(), beforeNode);
				if (it != parent->m_children.end()) {
					parent->m_children.insert(it, node);
				} else {
					parent->m_children.push_back(node);
				}
			} else {
				parent->m_children.push_back(node);
			}
			node->m_parent = parent;
		}
	}

	bool HierarchyTree::isAncestor(SceneGraphComponent* possibleParent, SceneGraphComponent* node) {
		while (node != nullptr) {
			if (node == possibleParent)
				return true;
			node = node->m_parent;
		}
		return false;
	}

	std::vector<Entity>& HierarchyTree::getSelectedEntities() {
		m_selectedEntities.clear();

		for (auto& node : getSelectedNodes()) {
			m_selectedEntities.push_back(node->m_entity);
		}
		return m_selectedEntities;
	}
}