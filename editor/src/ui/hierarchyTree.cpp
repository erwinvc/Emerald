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

namespace emerald {
	void HierarchyTree::renderNode(Scene* scene, SceneGraphComponent* node, int depth) {
		ImGui::PushID(ImGui::GetID((void*)(intptr_t)node));
		bool isRootNode = depth == 0;
		NameComponent* nameComponent = scene->getECS().getComponent<NameComponent>(node->m_entity);
		const std::string& name = (isRootNode ? scene->getName() : nameComponent->m_name);
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_NavLeftJumpsBackHere | ImGuiTreeNodeFlags_AllowItemOverlap;

		if (isRootNode) ImGui::Indent();
		if (node->m_children.empty()) {
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		
		if (!isRootNode && isNodeSelected(node)) {
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		flags |= ImGuiTreeNodeFlags_FramePadding;

		if (!isRootNode && ImGui::IsDragDropActive()) {
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			ImGui::PushItemFlag(ImGuiItemFlags_NoNav, true);
			ImGui::InvisibleButton(("##insert_before_" + name).c_str(), ImVec2(-1, 4), ImGuiButtonFlags_AllowOverlap);
			ImGui::PopItemFlag();
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
					//SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;
					for (auto& droppedNode : selectedNodes) {
						if (droppedNode && node && node != droppedNode) {
							if (!isAncestor(droppedNode, node)) {
								addNodeToParent(droppedNode, node->m_parent, true, node);
							}
						}
					}
				}
				ImGui::EndDragDropTarget();
			}
		}

		ImGui::SetNextItemOpen(node->m_isOpenInHierarchy || isRootNode);

		if (isRootNode) {
			flags |= ImGuiTreeNodeFlags_Framed;
			ImGui::PushStyleColor(ImGuiCol_Header, Color(0.07f, 0.07f, 0.07f, 1.0f));
		}

		bool wasOpen = node->m_isOpenInHierarchy = ImGui::TreeNodeEx((void*)(intptr_t)node, flags, name.c_str());

		onRightClick(node);

		if (isRootNode) {
			ImGui::PopStyleColor();
		}

		//Still have to figure out how to work with keyboard navigation
		//if (ImGui::IsItemFocused() && !ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		//	selectedNodes.clear();
		//	selectedNodes.push_back(node);
		//	lastSelectedNode = node;
		//}

		//if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
			//draggedNode = node;
			//isDragging = true;
		//}

		onDrag(node);
		onDrop(node);
		onLeftClick(node);

		if (wasOpen) {
			for (auto& child : node->m_children) {
				renderNode(scene, child, depth + 1);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	void HierarchyTree::onDrag(SceneGraphComponent* node) {
		if (ImGui::BeginDragDropSource()) {
			ImGui::SetDragDropPayload("NodeDragDrop", &node, sizeof(SceneGraphComponent*), ImGuiCond_Once);
			std::string name = "";
			for (int i = 0; i < selectedNodes.size(); i++) {
				NameComponent* nameComponent = SceneManager::getActiveScene()->getECS().getComponent<NameComponent>(selectedNodes[i]->m_entity); //This is ridiculously inefficient
				name += nameComponent->m_name;
				if (i < selectedNodes.size() - 1) {
					name += "\n";
				}
			}
			ImGui::Text("%s", name.c_str());
			ImGui::EndDragDropSource();
		}
	}

	void HierarchyTree::onDrop(SceneGraphComponent* node) {
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
				//SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;

				for (auto& droppedNode : selectedNodes) {
					if (droppedNode && node && node != droppedNode) {
						if (!isAncestor(droppedNode, node)) {
							addNodeToParent(droppedNode, node, false, node);
						}
					}
				}
				node->m_isOpenInHierarchy = true;
			}
			ImGui::EndDragDropTarget();
		}
	}

	void HierarchyTree::onLeftClick(SceneGraphComponent* node) {
		handleClickSelection(node, MouseButton::LEFT, false);
	}


	void HierarchyTree::onRightClick(SceneGraphComponent* node) {
		handleClickSelection(node, MouseButton::RIGHT, true);

		ImGui::PushEmeraldPopupStyle();
		if (ImGui::BeginPopup("HierarchyNodeOptions")) {
			ImGui::Selectable("Cut");
			ImGui::Selectable("Copy");
			ImGui::Selectable("Paste");
			ImGui::Separator();
			ImGui::Dummy(ImVec2(0, 1));
			ImGui::Selectable("Rename");
			ImGui::Selectable("Duplicate");
			ImGui::Selectable("Delete");
			ImGui::EndPopup();
		}
		ImGui::PopEmeraldPopupStyle();
	}

	void HierarchyTree::handleClickSelection(SceneGraphComponent* node, MouseButton button, bool openPopup) {
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && Mouse::buttonJustDown(button)) { //On just pressed
			if (openPopup) ImGui::OpenPopup("HierarchyNodeOptions"); //Open popup if right clicked
			if (Keyboard::keyMod(KeyMod::SHIFT) && lastSelectedNode != nullptr) { //If shift is pressed, select range
				selectRange(lastSelectedNode, node);
			} else {
				bool nodeSelected = isNodeSelected(node);
				if (Keyboard::keyMod(KeyMod::CONTROL)) {
					if (nodeSelected) {
						removeSelectedNode(node, true); //If clicked on a node that is selected, deselect it
					} else {
						addSelectedNode(node, true); //If clicked on a node that is not selected, select it
						lastSelectedNode = node;
					}
				} else {
					if (!nodeSelected) selectedNodes.clear(); //If clicked on a node that is not selected, clear selection
					addSelectedNode(node, true); //And select the clicked node
					lastSelectedNode = node;
				}
			}
		}

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_None) && Mouse::buttonJustUp(button)) { //On just released
			if (!Keyboard::keyMod(KeyMod::CONTROL) && !Keyboard::keyMod(KeyMod::SHIFT)) { //If no modifiers are selected, clear selection and select
				selectedNodes.clear();
				addSelectedNode(node, true);
				lastSelectedNode = node;
			}
		}
	}

	void HierarchyTree::sortSelectedNodes() {
		std::vector<SceneGraphComponent*> allNodes;
		std::function<void(SceneGraphComponent*)> collectNodes = [&](SceneGraphComponent* node) {
			allNodes.push_back(node);
			for (auto& child : node->m_children) {
				collectNodes(child);
			}
		};

		collectNodes(&SceneManager::getActiveScene()->getECS().getRootNode());

		std::unordered_map<SceneGraphComponent*, size_t> nodeIndexMap;
		for (size_t i = 0; i < allNodes.size(); ++i) {
			nodeIndexMap[allNodes[i]] = i;
		}

		std::sort(selectedNodes.begin(), selectedNodes.end(), [&](SceneGraphComponent* a, SceneGraphComponent* b) {
			return nodeIndexMap.at(a) < nodeIndexMap.at(b);
		});
	}

	void HierarchyTree::selectRange(SceneGraphComponent* startNode, SceneGraphComponent* endNode) {
		std::vector<SceneGraphComponent*> allNodes;
		std::function<void(SceneGraphComponent*)> collectNodes = [&](SceneGraphComponent* node) {
			allNodes.push_back(node);
			for (auto& child : node->m_children) {
				collectNodes(child);
			}
		};

		collectNodes(&SceneManager::getActiveScene()->getECS().getRootNode());

		selectedNodes.clear();

		bool foundStart = false;
		bool foundEnd = false;

		for (auto& node : allNodes) {
			if (!foundStart && !foundEnd) {
				if (node == startNode) {
					foundStart = true;
					addSelectedNode(node, false);
				}
				if (node == endNode) {
					foundEnd = true;
					addSelectedNode(node, false);
				}
			}

			if (foundStart && foundEnd) break;

			if (foundStart && !foundEnd) {
				addSelectedNode(node, false);
				if (endNode == node) break;
			}

			if (foundEnd && !foundStart) {
				addSelectedNode(node, false);
				if (startNode == node) break;
			}
		}

		sortSelectedNodes();
	}

	void HierarchyTree::addSelectedNode(SceneGraphComponent* node, bool sort) {
		if (!isNodeSelected(node))
			selectedNodes.push_back(node);
		if (sort)sortSelectedNodes();
	}

	void HierarchyTree::removeSelectedNode(SceneGraphComponent* node, bool sort) {
		utils::eraseFromVector(selectedNodes, node);
		if (sort)sortSelectedNodes();
	}

	bool HierarchyTree::isNodeSelected(SceneGraphComponent* node) {
		return std::find(selectedNodes.begin(), selectedNodes.end(), node) != selectedNodes.end();
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

	void HierarchyTree::render(Ref<Scene> scene) {

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::ItemRowsBackground(18);

		renderNode(scene.raw(), &SceneManager::getActiveScene()->getECS().getRootNode(), 0);

		// Handle dragging outside of any node (unparenting)
			//if (ImGui::BeginDragDropTarget()) {
			//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("NodeDragDrop")) {
			//		SceneGraphComponent* droppedNode = *(SceneGraphComponent**)payload->Data;
			//		dragTargetNode = nullptr;
			//		dragTargetParent = nullptr;
			//		isInsertingBefore = false;
			//	}
			//	ImGui::EndDragDropTarget();
			//}

		ImGui::PopStyleVar(2);
	}
}