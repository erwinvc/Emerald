#include "eepch.h"
#include "util/color.h"
#include "editor.h"
#include "entities/componentArray.h"
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

namespace emerald {
	HierarchyTree::HierarchyTree() {
		m_imGuiSelection.PreserveOrder = false;
		m_imGuiSelection.UserData = (void*)&m_nodes;
		m_imGuiSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			auto& nodes = *(std::vector<SceneGraphComponent*>*)self->UserData;
			return nodes[idx]->m_id;
		};
	}

	void HierarchyTree::render(Ref<Scene> scene, const char* searchString) {
		// Apply ImGui style settings
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::ItemRowsBackground(18, IM_COL32(28, 28, 28, 255));

		// Collect nodes
		m_nodes.clear();
		collectNodes(SceneManager::getActiveScene()->getRootNode());

		// Begin multi-selection and render nodes
		auto* multiSelectIO = ImGui::BeginMultiSelect(ImGuiMultiSelectFlags_None, m_imGuiSelection.Size, m_nodes.size());
		m_imGuiSelection.ApplyRequests(multiSelectIO);
		renderNode(scene.raw(), SceneManager::getActiveScene()->getRootNode(), searchString);
		onDrop(SceneManager::getActiveScene()->getRootNode(), true, SceneManager::getActiveScene()->getRootNode(), true);
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
			for (auto& node : selectedNodes) {
				SceneManager::getActiveScene()->getECS().destroyEntity(node->m_entity);
			}
		}
	}

	void HierarchyTree::collectNodes(SceneGraphComponent* node) {
		uint32_t index = 0;

		std::function<void(SceneGraphComponent*)> _collectNodes = [&](SceneGraphComponent* node) {
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
		bool isRootNode = depth == 0;
		NameComponent* nameComponent = scene->getECS().getComponent<NameComponent>(node->m_entity);
		const std::string& name = (isRootNode ? scene->getName() : nameComponent->m_name);
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

		node->m_isOpenInHierarchy = ImGui::TreeNodeEx((void*)(intptr_t)node->m_id, flags, name.c_str());

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
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
				ImVector<SceneGraphComponent*> selectedNodes = getSelectedNodes();

				auto action = UndoRedo::createAction<void>("Move nodes");

				// Capture the original state before making changes
				for (auto& droppedNode : selectedNodes) {
					if (droppedNode && node && node != droppedNode) {
						if (!isAncestor(droppedNode, node)) {
							addNodeToParent(droppedNode, node, insertBefore, beforeNode);
							// Capture the original parent and position of the node
							SceneGraphComponent* originalParent = droppedNode->m_parent;
							auto originalPosition = utils::getIndexInVector(originalParent->m_children, droppedNode);

							action->addUndoAction([droppedNode, originalParent, originalPosition]() {
								droppedNode->setParent(nullptr);
								if (originalParent) {
									originalParent->m_children.push_back(droppedNode);
									droppedNode->m_parent = originalParent;
									originalParent->sortChildrenBasedOnIndex();
								}
							});
							//addNodeToParent(droppedNode, node, insertBefore, beforeNode);

							uint32_t droppedNodeEntity = droppedNode->m_entity;
							uint32_t nodeEntity = node->m_entity;
							uint32_t beforeNodeEntity = beforeNode ? beforeNode->m_entity : Entity();
							action->addDoAction([this, droppedNodeEntity, nodeEntity, insertBefore, beforeNodeEntity]() {
								SceneGraphComponent* droppedNode1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(droppedNodeEntity);
								SceneGraphComponent* node1 = nodeEntity == 0 ? SceneManager::getActiveScene()->getRootNode() : SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(nodeEntity);
								SceneGraphComponent* beforeNode1 = SceneManager::getActiveScene()->getECS().getComponent<SceneGraphComponent>(beforeNodeEntity);
								addNodeToParent(droppedNode1, node1, insertBefore, beforeNode1);
							});

							//addNodeToParent(droppedNode, node, insertBefore, beforeNode);
							//Log::info("b {} {} {}", (uint64_t)droppedNode, (uint64_t)node, (uint64_t)beforeNode);
						}
					}
				}

				// Finalize and commit the action to the undo/redo stack
				UndoRedo::commitAction(action);

				if (open) node->m_isOpenInHierarchy = true;
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
}