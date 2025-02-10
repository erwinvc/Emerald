#include "eepch.h"
#include "hierarchyTree.h"
#include "ui/imguiManager.h"
#include "ui/iconsFontAwesome.h"
#include "imguiProfiler/IconsFontAwesome4.h"
#include "ui/iconsFontSegoeMDL2.h"
#include "graphics/misc/DPI.h"
#include "utils/math/color.h"
#include "engine/input/keyboard.h"
#include "engine/ecs/components/metadataComponent.h"
#include "utils/misc/utils.h"
#include "engine/scene/sceneManager.h"
#include "utils/uuid/uuid.h"
#include "core/project.h"
#include "utils/undoRedo.h"
#include "core/projectManager.h"
#include "core/selection.h"

namespace emerald {
	HierarchyTree::HierarchyTree() {
		m_imGuiSelection.PreserveOrder = false;
		m_imGuiSelection.UserData = (void*)&m_nodes;
		m_imGuiSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			auto& nodes = *(std::vector<SceneGraphComponent*>*)self->UserData;
			return nodes[idx]->m_id;
		};
	}

	struct TableHeader {
		std::string_view name;
		ImGUIFont font;
	};

	void HierarchyTree::draw(const Ref<Scene>& scene, const char* searchString) {
		// Apply ImGui style settings
		ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
		ImGui::ItemRowsBackground(18, IM_COL32(28, 28, 28, 255));
		static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_Resizable | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX | ImGuiTableFlags_NoHostExtendX;
		static ImGuiTreeNodeFlags tree_node_flags = ImGuiTreeNodeFlags_SpanAllColumns;
		static TableHeader headers[4] = {
			{ "Name", ImGUIFont::INTER },
			{ (const char*)SEGOE_MDL2_ICON_DIAL_SHAPE1, ImGUIFont::SEGOE },
			{ "Tag", ImGUIFont::INTER },
			{  P_ICON_FA_EYE, ImGUIFont::AWESOME_S }
		};

		if (ImGui::BeginTable("hierarchyTreeTable", 4, flags)) {
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_NoHide | ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, DPI::getScale(19.0f));
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, DPI::getScale(38.0f));
			ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, DPI::getScale(19.0f));

			ImGui::TableNextRow(ImGuiTableRowFlags_Headers);
			for (int column = 0; column < 4; column++) {
				ImGui::TableSetColumnIndex(column);
				ImGui::PushID(column);
				ImGuiManager::pushFont(headers[column].font);
				ImGui::TableHeader(headers[column].name.data());
				ImGuiManager::popFont();
				ImGui::PopID();
			}

			// Collect nodes
			m_nodes.clear();
			if (ProjectManager::hasOpenProject()) collectNodes(scene->getRootNode());

			// Begin multi-selection and render nodes
			auto* multiSelectIO = ImGui::BeginMultiSelect(ImGuiMultiSelectFlags_None, (int)m_imGuiSelection.Size, (int)m_nodes.size());
			m_imGuiSelection.ApplyRequests(multiSelectIO);
			renderNode(scene->getRootNode(), searchString);

			if (ImGui::BeginDragDropTarget()) {
				onDrop(scene->getRootNode(), true, scene->getRootNode(), true);
				ImGui::EndDragDropTarget();
			}
			multiSelectIO = ImGui::EndMultiSelect();
			m_imGuiSelection.ApplyRequests(multiSelectIO);
			m_isFocused = ImGui::IsWindowFocused();
			ImGui::EndTable();
		}

		// Restore ImGui style settings
		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor();

		updateSelectedEntities();
	}

	void HierarchyTree::handleDelete() {
		if (m_isFocused && Keyboard::keyJustDown(Key::DEL)) {
			auto selectedNodes = getSelectedNodes();
			//auto action = UndoRedo::createAction<void>("Delete entities");
			for (auto& node : selectedNodes) {
				if (!node) continue;
				if (node->m_entity != SceneManager::getActiveScene()->getRootNode()->m_entity) {
					ECSManager::ECS().destroyEntity(node->m_entity);
				}
			}
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
			//UndoRedo::commitAction(action);
		}
	}

	void HierarchyTree::collectNodes(SceneGraphComponent* node) {
		uint32_t index = 0;

		static std::function<void(SceneGraphComponent*)> _collectNodes = [&](SceneGraphComponent* node) {
			m_nodes.push_back(node);
			node->m_treeIndex = index++;
			node->m_id = ImGui::ptrToImGuiID(node);
			for (auto& child : node->m_children) {
				_collectNodes(child);
			}
		};

		_collectNodes(node);
	}

	ImRect HierarchyTree::renderNode(SceneGraphComponent* node, const char* searchString, int depth) {
		if (!node) return ImRect(ImVec4(0, 0, 0, 0));
		bool isRootNode = depth == 0;
		MetadataComponent* metadata = ECSManager::ECS().getComponent<MetadataComponent>(node->m_entity);

		ImGuiTreeNodeFlags flags = prepareTreeNodeFlags(node, isRootNode);
		ImGui::SetNextItemOpen(node->m_isOpenInHierarchy || isRootNode);

		// Begin table row
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (!isRootNode && ImGui::IsDragDropActive()) {
			drawInsertBeforeDropTarget();
			if (ImGui::BeginDragDropTarget()) {
				onDrop(node->m_parent, true, node, false);
				ImGui::EndDragDropTarget();
			}
		}

		if (!ImGui::IsDragDropActive()) {
			ImGui::SetNextItemSelectionUserData(utils::getIndexInVector(m_nodes, node));
		}

		if (isRootNode) {
			//ImGui::Indent();
			ImGui::PushStyleColor(ImGuiCol_Header, Color(0.07f, 0.07f, 0.07f, 1.0f));
		}

		node->m_isOpenInHierarchy = ImGui::TreeNodeEx(&node->m_id, flags, metadata ? metadata->getName().c_str() : "NULL");
		const ImRect nodeRect = ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
		ImVec2 verticalLineStart = ImGui::GetCursorScreenPos();

		if (ImGui::IsItemFocused()) {
			m_lastSelectedNode = node;
		}

		bool skip = false;
		if (!isRootNode) {
			onDrag(node);

			if (ImGui::BeginDragDropTarget()) {
				skip = onDrop(node, false, node, true);
				ImGui::EndDragDropTarget();
			}
			onRightClick(node);
		}

		ImGui::TableNextColumn();

		if (!isRootNode) {
			static bool b;
			ImGui::PushID((void*)(intptr_t)(node->m_id + 1));
			ImGui::TableNextColumn();
			ImGui::TextDisabled("--");
			ImGui::TableNextColumn();
			ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorU32(ImGuiCol_TextDisabled));
			ImGuiManager::pushFont(ImGUIFont::AWESOME_R);
			bool entityEnabled = ECSManager::ECS().isEntityEnabled(node->m_entity);
			if (ImGui::ToggleButton("", &entityEnabled, ImVec2(0, ImGui::GetTextLineHeight() + 2), P_ICON_FA_EYE, P_ICON_FA_EYE_SLASH)) {
				auto it = std::find(m_selectedEntities.begin(), m_selectedEntities.end(), node->m_entity);
				if (it != m_selectedEntities.end()) {
					for (auto& selectedEntity : m_selectedEntities) {
						SceneGraphComponent* sgc = ECSManager::ECS().getComponent<SceneGraphComponent>(selectedEntity);
						sgc->setEnabledRecursive(entityEnabled);
					}
				} else {
					SceneGraphComponent* sgc = ECSManager::ECS().getComponent<SceneGraphComponent>(node->m_entity);
					sgc->setEnabledRecursive(entityEnabled);
				}

			}
			ImGuiManager::popFont();
			ImGui::PopStyleColor();
			ImGui::TableNextColumn();

			ImGui::PopID();
		}

		if (isRootNode) {
			ImGui::PopStyleColor();
		}

		const ImColor TreeLineColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
		const float SmallOffsetX = -0.0f; //for now, a hardcoded value; should take into account tree indent size
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		verticalLineStart.x += SmallOffsetX; //to nicely line up with the arrow symbol
		ImVec2 verticalLineEnd = verticalLineStart;
		if (node->m_isOpenInHierarchy) {
			if (!skip) {
				for (auto* child : node->m_children) {
					const float HorizontalTreeLineSize = 8.0f; //chosen arbitrarily
					ImRect childRect = renderNode(child, searchString, depth + 1);
					const float midpoint = (childRect.Min.y + childRect.Max.y) / 2.0f;
					drawList->AddLine(ImVec2(verticalLineStart.x, midpoint), ImVec2(verticalLineStart.x + HorizontalTreeLineSize, midpoint), TreeLineColor);
					verticalLineEnd.y = midpoint;
				}
			}
			ImGui::TreePop();
		}

		drawList->AddLine(verticalLineStart, verticalLineEnd, TreeLineColor);
		return nodeRect;
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
			ImGuiTreeNodeFlags_SpanAllColumns |
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
				auto* metadata = ECSManager::ECS().getComponent<MetadataComponent>(selectedNodes[i]->m_entity);
				name += metadata->getName();
				if (i < selectedNodes.size() - 1) name += "\n";
			}

			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}
	}

	bool HierarchyTree::onDrop(SceneGraphComponent* node, bool insertBefore, SceneGraphComponent* beforeNode, bool open) {
		if (!node) return false;
		bool toRet = false;
		ImVector<SceneGraphComponent*> selectedNodes = getSelectedNodes();

		auto action = UndoRedo::createAction<void>("Move nodes");
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
			for (auto& droppedNode : selectedNodes) {
				if (droppedNode && node != droppedNode) {
					if (!isAncestor(droppedNode, node)) {
						toRet = true;
						SceneGraphComponent* originalParent = droppedNode->m_parent;
						auto originalPosition = utils::getIndexInVector(originalParent->m_children, droppedNode);

						SceneGraphComponent* originalPrevSibling = originalPosition > 0 ? originalParent->m_children[originalPosition - 1] : nullptr;
						SceneGraphComponent* originalNextSibling = (originalPosition < originalParent->m_children.size() - 1) ? originalParent->m_children[originalPosition + 1] : nullptr;

						addNodeToParent(droppedNode, node, insertBefore, beforeNode);

						UUID beforeNodeEntity = beforeNode ? beforeNode->m_entity : Entity();

						action->addUndoAction([droppedNodeEntity = droppedNode->m_entity, originalParentEntity = originalParent->m_entity, originalPrevSibling, originalNextSibling]() {
							SceneGraphComponent* droppedNode1 = ECSManager::ECS().getComponent<SceneGraphComponent>(droppedNodeEntity);
							SceneGraphComponent* originalParent1 = ECSManager::ECS().getComponent<SceneGraphComponent>(originalParentEntity);

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
							SceneGraphComponent* droppedNode1 = ECSManager::ECS().getComponent<SceneGraphComponent>(droppedNodeEntity);
							SceneGraphComponent* node1 = ECSManager::ECS().getComponent<SceneGraphComponent>(nodeEntity);
							SceneGraphComponent* beforeNode1 = beforeNodeEntity.isValid() ? ECSManager::ECS().getComponent<SceneGraphComponent>(beforeNodeEntity) : nullptr;
							addNodeToParent(droppedNode1, node1, insertBefore, beforeNode1);
						});

						UndoRedo::commitAction(action);
					}
				}
			}
		}

		if (open && node) node->m_isOpenInHierarchy = true;
		return toRet;
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

	void HierarchyTree::updateSelectedEntities() {
		Selection::clearSelection();
		m_selectedEntities.clear();

		for (auto& node : getSelectedNodes()) {
			if (node->m_entity == SceneManager::getActiveScene()->getRootNode()->m_entity) continue;
			m_selectedEntities.push_back(node->m_entity);
			Selection::selectEntity(node->m_entity);
		}
	}
}