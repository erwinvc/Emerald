#include "eepch.h"
#include "assetBrowserPanel.h"
#include "ui/imguiManager.h"
#include "project.h"
#include "engine/events/eventSystem.h"
#include "ui/iconsFontAwesome.h"
#include "engine/assets/core/assetRegistry.h"
#include "editor.h"
#include "utils/system/fileSystem.h"
#include "graphics/textures/texture.h"
#include "graphics/misc/DPI.h"
#include "graphics/core/renderer.h"
#include "engine/input/keyboard.h"
#include "input/dragDrop.h"
#include "engine/assets/loaders/textureLoader.h"
#include "utils/system/timer.h"
#include "utils/math/color.h"
#include "utils/datastructures/vector.h"

namespace emerald {
	static constexpr float MIN_CELL_SIZE = 50.0f;
	static constexpr float MAX_CELL_SIZE = 300.0f;

	static std::unordered_map<AssetType, Ref<Texture>> s_assetTypeIcons;

	AssetBrowserPanel::AssetBrowserPanel() {
		m_imGuiSelection.PreserveOrder = false;
		m_imGuiSelection.UserData = this;
		m_imGuiSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			AssetBrowserPanel& panel = *(AssetBrowserPanel*)self->UserData;
			return panel.filteredContents[idx].m_imGuiID;
		};

		EventSystem::subscribe<EditorProjectOpenedEvent>(&AssetBrowserPanel::onProjectOpened, this);
		EventSystem::subscribe<MouseButtonEvent>(&AssetBrowserPanel::onMouseButtonEvent, this);

		TextureDesc desc;
		desc.name = "AssetBrowserPanelIcon";
		s_assetTypeIcons[AssetType::DEFAULT] = TextureLoader(desc, "res/textures/default.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::FOLDER] = TextureLoader(desc, "res/textures/folder.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::PREFAB] = TextureLoader(desc, "res/textures/prefab.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::MATERIAL] = TextureLoader(desc, "res/textures/material.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::SHADER] = TextureLoader(desc, "res/textures/shader.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::TEXTURE] = TextureLoader(desc, "res/textures/texture.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::SCENE] = TextureLoader(desc, "res/textures/scene.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::MODEL] = TextureLoader(desc, "res/textures/model.png", false).loadAndInvalidate();
		s_assetTypeIcons[AssetType::AUDIO] = TextureLoader(desc, "res/textures/audio.png", false).loadAndInvalidate();

		m_folderIcon = TextureLoader(desc, "res/textures/folder.png", false).loadAndInvalidate();
		m_folderEmptyIcon = TextureLoader(desc, "res/textures/folderEmpty.png", false).loadAndInvalidate();
	}

	void AssetBrowserPanel::onProjectOpened(EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			m_currentPath = Project::GetAssetsPath();
			std::stack<std::filesystem::path>().swap(m_forwardStack);
		}
	}

	void AssetBrowserPanel::onMouseButtonEvent(MouseButtonEvent& e) {
		if (!m_windowHovered) return;
		if (e.isPressed()) {
			if (e.getButton() == MouseButton::BACK) {
				navigateBack();
			} else if (e.getButton() == MouseButton::FORWARD) {
				navigateForward();
			}
		}
	}

	void AssetBrowserPanel::navigateBack() {
		if (m_currentPath != Project::GetAssetsPath()) {
			m_forwardStack.push(m_currentPath);
			m_currentPath = m_currentPath.parent_path();
			if (m_currentPath < Project::GetAssetsPath()) {
				m_currentPath = Project::GetAssetsPath();
			}
		}
	}

	void AssetBrowserPanel::navigateForward() {
		if (!m_forwardStack.empty()) {
			m_currentPath = m_forwardStack.top();
			m_forwardStack.pop();
		}
	}

	void AssetBrowserPanel::setCurrentPath(const std::filesystem::path& newPath) {
		m_imGuiSelection.Clear();
		if (newPath != m_currentPath) {
			while (!m_forwardStack.empty()) {
				m_forwardStack.pop();
			}
			m_currentPath = newPath;
		}
	}

	std::vector<AssetBrowserPanel::AssetEntry> AssetBrowserPanel::getFilteredDirectoryContents() {
		filteredContents.clear();
		if (!Project::isProjectOpen() || !std::filesystem::is_directory(m_currentPath)) return filteredContents;

		bool isTypeSearch = std::string(m_searchBuffer).starts_with("type:");
		bool all = std::string(m_searchBuffer) == "*";
		if (strlen(m_searchBuffer) > 0) {
			for (const auto& entry : std::filesystem::recursive_directory_iterator(m_currentPath)) {
				if (entry.path().filename().string().starts_with('.'))
					continue;

				AssetMetadata* metadata = AssetRegistry::getAssetMetadata(entry.path());
				if (!metadata) continue;

				if (all) {
					filteredContents.emplace_back(entry.path(), ImGui::ptrToImGuiID(metadata), entry.is_directory(), metadata);
					continue;
				}

				if (isTypeSearch) {
					std::string searchType = std::string(m_searchBuffer).substr(5);
					std::transform(searchType.begin(), searchType.end(), searchType.begin(), ::tolower);

					std::string assetType = assetTypeToString(metadata->getType(), false);
					std::transform(assetType.begin(), assetType.end(), assetType.begin(), ::tolower);

					if (assetType.find(searchType) == std::string::npos)
						continue;
				} else {
					std::string lowercaseSearch = m_searchBuffer;
					std::transform(lowercaseSearch.begin(), lowercaseSearch.end(), lowercaseSearch.begin(), ::tolower);
					std::string lowercaseFilename = entry.path().filename().string();
					std::transform(lowercaseFilename.begin(), lowercaseFilename.end(), lowercaseFilename.begin(), ::tolower);

					if (lowercaseFilename.find(lowercaseSearch) == std::string::npos)
						continue;
				}
				filteredContents.emplace_back(entry.path(), ImGui::ptrToImGuiID(metadata), entry.is_directory(), metadata);
			}
		} else {
			for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
				if (entry.path().filename().string().starts_with('.'))
					continue;

				AssetMetadata* metadata = AssetRegistry::getAssetMetadata(entry);
				if (!metadata) continue;

				filteredContents.emplace_back(entry.path(), ImGui::ptrToImGuiID(metadata), entry.is_directory(), metadata);
			}
		}

		// Sort: directories first, then alphabetically
		std::sort(filteredContents.begin(), filteredContents.end(),
			[](const AssetEntry& a, const AssetEntry& b) {
			if (a.m_isDirectory && !b.m_isDirectory) return true;
			if (!a.m_isDirectory && b.m_isDirectory) return false;
			return a.m_path.filename() < a.m_path.filename();
		});

		return filteredContents;
	}

	void AssetBrowserPanel::renderDirectoryTree(const std::filesystem::path& path) {
		ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAllColumns |
			ImGuiTreeNodeFlags_FramePadding;

		if (path == Project::GetAssetsPath()) {
			rootFlags |= ImGuiTreeNodeFlags_DefaultOpen;
		}

		if (path == m_currentPath) {
			rootFlags |= ImGuiTreeNodeFlags_Selected;
		}
		bool hasSubdirectories = false;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (std::filesystem::is_directory(entry)) {
				hasSubdirectories = true;
				break;
			}
		}

		if (!hasSubdirectories) rootFlags |= ImGuiTreeNodeFlags_Leaf;

		bool rootNodeOpen = ImGui::TreeNodeEx(path.filename().string().c_str(), rootFlags);

		if ((ImGui::IsItemClicked()) && m_currentPath != path) {
			setCurrentPath(path);
		}

		if (rootNodeOpen) {
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				if (!std::filesystem::is_directory(entry)) continue;

				bool isCurrentPath = entry.path() == m_currentPath;

				renderDirectoryTree(entry.path());
			}

			ImGui::TreePop();
		}
	}

	void AssetBrowserPanel::renderAssetGrid() {
		auto directoryContents = getFilteredDirectoryContents();

		const float padding = 10.0f;
		int columnsCount = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / (m_cellSize + padding)));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));
		ImGui::Columns(columnsCount, "AssetColumns", false);

		uint32_t fileIndex = 0;
		for (const auto& file : directoryContents) {
			const int maxNameLines = 2;
			const float nameAssetTypePadding = 5.0f;
			const float bottomPadding = 5.0f;

			ImGui::PushID(file.m_path.string().c_str());
			ImGui::BeginGroup();
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 pos = ImGui::GetCursorScreenPos();

			ImVec2 cardSize = ImVec2(m_cellSize, m_cellSize + ImGui::GetTextLineHeight() * (maxNameLines + 1) + nameAssetTypePadding + bottomPadding);

			// Background
			drawList->AddRectFilled(
				ImVec2(pos.x, pos.y),
				ImVec2(pos.x + cardSize.x, pos.y + cardSize.y),
				ImGui::GetColorU32(ImGuiCol_Button),
				8.0f,
				ImDrawFlags_RoundCornersBottom
			);

			// Shadow
			drawList->AddRectFilled(
				ImVec2(pos.x, pos.y),
				ImVec2(pos.x + cardSize.x + 3, pos.y + cardSize.y + 4),
				IM_COL32(0, 0, 0, 128),
				8.0f,
				ImDrawFlags_RoundCornersBottom
			);

			m_idToPath[file.m_imGuiID] = file.m_path;

			ImGui::SetNextItemSelectionUserData(fileIndex);

			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_Header, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color::transparent());

			bool isSelected = m_imGuiSelection.Contains(file.m_imGuiID);
			ImGui::Selectable("##AssetCard", isSelected, 0, cardSize);

			ImGui::PopStyleColor(4);


			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID | ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
				Vector<UUID> selectedAssets;
				for (const auto& selectedAsset : getSelectedAssets()) {
					AssetMetadata* selectedAssetMetadata = AssetRegistry::getAssetMetadata(selectedAsset);
					if (selectedAssetMetadata && selectedAssetMetadata->getType() != AssetType::FOLDER) {
						AssetRegistry::streamAsset(selectedAssetMetadata);
					}
					selectedAssets.pushBack(selectedAssetMetadata->getUUID());
				}
				
				if (!selectedAssets.contains(file.m_metadata->getUUID())) {
					selectedAssets.pushBack(file.m_metadata->getUUID());
				}

				const size_t payloadSize = selectedAssets.size() * sizeof(UUID);
				ImGui::SetDragDropPayload(DRAG_DROP_ASSET, selectedAssets.data(), payloadSize);

				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Open in Explorer")) {
					FileSystem::openFolderAndSelectItem(file.m_path);
				}
				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (file.m_isDirectory) {
					setCurrentPath(file.m_path);
				}
			}

			ImVec2 thumbnailSize = ImVec2(m_cellSize, m_cellSize);

			ImGui::SetCursorScreenPos(pos);

			uint32_t icon = file.m_isDirectory ? std::filesystem::is_empty(file.m_path) ? m_folderEmptyIcon->handle() : m_folderIcon->handle() : s_assetTypeIcons[file.m_metadata->getType()]->handle();
			ImGui::Image((void*)(uint64_t)icon, DPI::getScale(thumbnailSize));

			if (isSelected) {
				drawList->AddRect(
					pos,
					ImVec2(pos.x + cardSize.x, pos.y + cardSize.y),
					ImGui::GetColorU32(ImGuiCol_NavHighlight),
					8.0f,
					ImDrawFlags_RoundCornersBottom,
					2.0f
				);
			}

			// Text rendering
			ImGui::SetCursorScreenPos(ImVec2(pos.x + 4, pos.y + thumbnailSize.y));

			// Filename
			float lineHeight = ImGui::GetTextLineHeight();
			float maxHeight = lineHeight * maxNameLines;
			ImVec2 textRegionSize = ImVec2(cardSize.x - 8, maxHeight);
			ImGui::BeginChild("TextRegion", textRegionSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs);

			ImGuiManager::pushFont(INTER_BOLD);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + cardSize.x - 8);
			ImGui::Text("%s", file.m_path.stem().string().c_str());
			ImGui::PopTextWrapPos();
			ImGuiManager::popFont();

			ImGui::EndChild();

			// Asset type
			const char* assetTypeText = assetTypeToString(file.m_metadata->getType(), true);
			ImVec2 textSize = ImGui::CalcTextSize(assetTypeText);

			ImVec2 cellMin = pos;
			ImVec2 cellMax = ImVec2(
				pos.x + cardSize.x,
				pos.y + cardSize.y
			);

			ImVec2 assetTypePos = ImVec2(
				pos.x + cardSize.x - textSize.x - 10, // 10px padding from right
				pos.y + cardSize.y - textSize.y - bottomPadding
			);

			ImGui::PushClipRect(cellMin, cellMax, true);
			ImGui::SetCursorScreenPos(assetTypePos);
			ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", assetTypeText);
			ImGui::PopClipRect();

			ImGui::PopStyleVar();
			ImGui::PopStyleColor();  // Border color
			ImGui::EndGroup();

			ImGui::NextColumn();
			ImGui::PopID();

			fileIndex++;
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();  // Item spacing
	}

	void AssetBrowserPanel::drawBreadcrumbPart(const std::string& label, uint32_t index, const std::filesystem::path& breadcrumbPath, bool isLast) {
		ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
		ImVec2 pos = ImGui::GetCursorScreenPos();

		if (isLast) {
			ImGuiManager::pushFont(INTER_BOLD);
			ImGui::Text("%s", label.c_str());
			ImGuiManager::popFont();
		} else {
			ImGui::PushID(label.c_str());
			ImGui::InvisibleButton(ImGuiManager::generateLabel("##invisible_btn", index), textSize);
			bool hovered = ImGui::IsItemHovered();
			bool clicked = ImGui::IsItemClicked();
			ImGui::PopID();

			ImU32 textColor = clicked ? ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
				: (hovered ? ImGui::GetColorU32(ImGuiCol_Text) : ImGui::GetColorU32(ImGuiCol_TextDisabled));

			ImGui::GetWindowDrawList()->AddText(pos, textColor, label.c_str());

			if (clicked) {
				setCurrentPath(breadcrumbPath);
			}
		}

		ImGui::SetCursorScreenPos(ImVec2(pos.x + textSize.x, pos.y));
	}

	void AssetBrowserPanel::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav)) {
			static float splitRatio = 0.25f;

			ImGui::BeginGroup();
			{
				// Left Panel: Directory Tree
				ImGui::BeginChild("Directories", ImVec2(ImGui::GetContentRegionAvail().x * splitRatio, -1), ImGuiChildFlags_Border);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				if (Project::isProjectOpen()) renderDirectoryTree(Project::GetAssetsPath());
				ImGui::PopStyleVar(2);
				ImGui::EndChild();

				ImGui::SameLine();

				// Right Panel: Asset Grid
				if (ImGui::BeginChild("Assets", ImVec2(0, -1), true)) {

					if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows)) {
						if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl)) {
							m_cellSize += ImGui::GetIO().MouseWheel * 10.0f;
							int roundedCellSize = (int)m_cellSize;
							m_cellSize = std::clamp((float)roundedCellSize, MIN_CELL_SIZE, MAX_CELL_SIZE);
						}
					}

					if (ImGui::BeginTable(UNIQUE_IMGUI_LABEL(), 4, ImGuiTableFlags_None)) {
						ImGui::TableSetupColumn("Breadcrumbs", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("CellSize", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableSetupColumn("InputText", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed);
						ImGui::TableNextRow();
						ImGui::TableSetColumnIndex(0);
						ImGui::SetNextItemWidth(-FLT_MIN);

						ImGui::BeginGroup();

						std::filesystem::path displayPath = m_currentPath.lexically_relative(Project::GetProjectPath());
						displayPath = displayPath.lexically_normal();

						std::filesystem::path currentBreadcrumb = Project::GetProjectPath();

						uint32_t index = 0;
						size_t breadcrumbCount = std::distance(displayPath.begin(), displayPath.end());
						for (const auto& part : displayPath) {
							ImGui::SameLine(0, 2.0f);
							if (index != 0) {
								ImGui::Text(">");
								ImGui::SameLine(0, 2.0f);
							}

							currentBreadcrumb /= part;
							std::string partString = part.string();
							drawBreadcrumbPart(partString, index++, currentBreadcrumb, index == breadcrumbCount - 1);

							ImGui::SameLine(0, 2.0f);
						}

						ImGui::EndGroup();

						ImGui::TableSetColumnIndex(1);

						ImGui::AlignTextToFramePadding();
						ImGui::Text("Cell size");
						ImGui::SameLine();
						ImGui::SliderFloat("##Cell Size", &m_cellSize, MIN_CELL_SIZE, MAX_CELL_SIZE, "%.0f", ImGuiSliderFlags_AlwaysClamp);

						ImGui::TableSetColumnIndex(2);

						ImGui::SetNextItemWidth(-FLT_MIN);
						ImGui::InputTextWithHint(UNIQUE_IMGUI_LABEL(), ICON_FA_SEARCH " Search...", m_searchBuffer, 256, ImGuiInputTextFlags_EscapeClearsAll);

						ImGui::TableSetColumnIndex(3);
						if (ImGui::Button(ICON_FA_TIMES)) {
							memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
						}

						ImGui::EndTable();
					}


					ImGui::BeginChild("##AssetGrid", ImVec2(0, -1), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
					ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_SelectOnClickRelease;
					ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, m_imGuiSelection.Size, (uint32_t)getFilteredDirectoryContents().size());
					m_imGuiSelection.ApplyRequests(ms_io);

					if (Project::isProjectOpen()) renderAssetGrid();

					ms_io = ImGui::EndMultiSelect();

					m_imGuiSelection.ApplyRequests(ms_io);
					if (ms_io->Requests.size() > 0) {
						updateSelectedAssets();
					}
					ImGui::EndChild();
				}

				ImGui::EndChild();
			}
			ImGui::EndGroup();
		}
		m_windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
		ImGui::End();
	}

	void AssetBrowserPanel::updateSelectedAssets() {
		m_selectedAssets.clear();
		void* iter = nullptr;
		ImGuiID selectedId = 0;
		while (m_imGuiSelection.GetNextSelectedItem(&iter, &selectedId)) {
			auto it = m_idToPath.find(selectedId);
			if (it != m_idToPath.end()) {
				m_selectedAssets.push_back(it->second);
			}
		}
	}
}