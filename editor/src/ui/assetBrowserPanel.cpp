#include "eepch.h"
#include "assetBrowserPanel.h"
#include "ui/imguiManager.h"
#include "engine/events/eventSystem.h"
#include "ui/iconsFontAwesome.h"
#include "engine/assets/core/assetRegistry.h"
#include "utils/system/fileSystem.h"
#include "graphics/textures/texture.h"
#include "graphics/misc/DPI.h"
#include "input/dragDrop.h"
#include "engine/assets/loaders/textureLoader.h"
#include "utils/math/color.h"
#include "utils/datastructures/vector.h"
#include "core/project.h"
#include "core/projectManager.h"

namespace emerald {
	static constexpr float MIN_CELL_SIZE = 50.0f;
	static constexpr float MAX_CELL_SIZE = 300.0f;

	static std::unordered_map<AssetType, Ref<Texture>> s_assetTypeIcons;

	AssetBrowserPanel::AssetBrowserPanel() {
		m_imGuiSelection.PreserveOrder = false;
		m_imGuiSelection.UserData = this;
		m_imGuiSelection.AdapterIndexToStorageId = [](ImGuiSelectionBasicStorage* self, int idx) {
			AssetBrowserPanel& panel = *(AssetBrowserPanel*)self->UserData;
			return panel.m_filteredContent[idx]->m_imGuiId;
		};

		EventSystem::subscribe<EditorProjectOpenedEvent>(&AssetBrowserPanel::onProjectOpened, this);
		EventSystem::subscribe<MouseButtonEvent>(&AssetBrowserPanel::onMouseButtonEvent, this);
		EventSystem::subscribe<FileChangedEvent>(&AssetBrowserPanel::onFileChangedEvent, this);

		TextureDesc desc;
		desc.name = "AssetBrowserPanelIcon";
		s_assetTypeIcons[AssetType::DEFAULT] = TextureLoader(desc, "res/textures/default.png", false).load();
		//s_assetTypeIcons[AssetType::FOLDER] = TextureLoader(desc, "res/textures/folder.png", false).load();
		s_assetTypeIcons[AssetType::PREFAB] = TextureLoader(desc, "res/textures/prefab.png", false).load();
		s_assetTypeIcons[AssetType::MATERIAL] = TextureLoader(desc, "res/textures/material.png", false).load();
		s_assetTypeIcons[AssetType::SHADER] = TextureLoader(desc, "res/textures/shader.png", false).load();
		s_assetTypeIcons[AssetType::TEXTURE] = TextureLoader(desc, "res/textures/texture.png", false).load();
		s_assetTypeIcons[AssetType::SCENE] = TextureLoader(desc, "res/textures/scene.png", false).load();
		s_assetTypeIcons[AssetType::MODEL] = TextureLoader(desc, "res/textures/model.png", false).load();
		s_assetTypeIcons[AssetType::AUDIO] = TextureLoader(desc, "res/textures/audio.png", false).load();

		m_folderIcon = TextureLoader(desc, L"res/textures/folder.png", false).load();
		m_folderEmptyIcon = TextureLoader(desc, "res/textures/folderEmpty.png", false).load();
	}

	void AssetBrowserPanel::onProjectOpened(EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			m_currentPath = ProjectManager::getCurrentProject().getAssetsFolder();
			std::stack<std::filesystem::path>().swap(m_forwardStack);
			m_updateDirectoryContentsNextFrame = true;
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

	void AssetBrowserPanel::onFileChangedEvent(FileChangedEvent& e) {
		if (e.getType() != FileChangedEventType::MODIFIED) {
			m_updateDirectoryContentsNextFrame = true;
		}
	}


	void AssetBrowserPanel::navigateBack() {
		if (m_currentPath != ProjectManager::getCurrentProject().getAssetsFolder()) {
			m_forwardStack.push(m_currentPath);
			m_currentPath = m_currentPath.parent_path();
			if (m_currentPath < ProjectManager::getCurrentProject().getAssetsFolder()) {
				m_currentPath = ProjectManager::getCurrentProject().getAssetsFolder();
			}
		}
		memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
		updateFilter();
	}

	void AssetBrowserPanel::navigateForward() {
		if (!m_forwardStack.empty()) {
			m_currentPath = m_forwardStack.top();
			m_forwardStack.pop();
		}
		memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
		updateFilter();
	}

	void AssetBrowserPanel::setCurrentPath(const std::filesystem::path& newPath) {
		m_imGuiSelection.Clear();
		if (newPath != m_currentPath) {
			while (!m_forwardStack.empty()) {
				m_forwardStack.pop();
			}
			m_currentPath = newPath;
		}
		memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
		updateFilter();
	}

	void AssetBrowserPanel::collectDirectoryContents(const std::filesystem::path& directoryPath) {
		std::vector<DirectoryContent> contentList;

		for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
			std::error_code ec;

			if (entry.path().filename().u8string().starts_with('.'))
				continue;

			if (entry.path().extension() == ".meta") {
				continue;
			}

			AssetMetadata* metadata = AssetRegistry::getAssetMetadata(entry.path());
			if (!metadata)
				continue;

			bool isDir = std::filesystem::is_directory(entry.path(), ec);

			if (ec) {
				// Something went wrong (file might be gone, permissions, etc.)
				continue;
			}

			if (isDir) {
				collectDirectoryContents(entry.path());
			}

			contentList.emplace_back(entry.path(), ImGui::ptrToImGuiID(metadata), entry.is_directory(), metadata);
		}

		m_directoryMap[directoryPath] = std::move(contentList);
	}

	void AssetBrowserPanel::updateDirectoryContents() {
		if (!ProjectManager::hasOpenProject()) return;

		m_directoryMap.clear();

		collectDirectoryContents(ProjectManager::getCurrentProject().getAssetsFolder());
		updateFilter();
	}

	void AssetBrowserPanel::updateFilter() {
		m_filteredContent.clear();

		if (!ProjectManager::hasOpenProject()) return;

		bool isTypeSearch = std::string(m_searchBuffer).starts_with("type:");
		bool all = std::string(m_searchBuffer) == "*";
		if (strlen(m_searchBuffer) > 0) {
			if (all) {
				for (auto& [path, contents] : m_directoryMap) {
					for (auto& content : contents) {
						m_filteredContent.push_back(&content);
					}
				}
			} else if (isTypeSearch) {
				std::string searchType = std::string(m_searchBuffer).substr(5);
				std::transform(searchType.begin(), searchType.end(), searchType.begin(), ::tolower);

				for (auto& [path, contents] : m_directoryMap) {
					for (auto& content : contents) {
						std::string assetType = assetTypeToString(content.m_metadata->getType(), false);
						std::transform(assetType.begin(), assetType.end(), assetType.begin(), ::tolower);

						if (assetType.find(searchType) != std::string::npos) {
							m_filteredContent.push_back(&content);
						}
					}
				}
			} else {
				std::string lowercaseSearch = m_searchBuffer;
				std::transform(lowercaseSearch.begin(), lowercaseSearch.end(), lowercaseSearch.begin(), ::tolower);

				for (auto& content : m_directoryMap[m_currentPath]) {
					std::string lowercaseFilename = FileSystem::pathToString(content.m_path.filename());
					std::transform(lowercaseFilename.begin(), lowercaseFilename.end(), lowercaseFilename.begin(), ::tolower);

					if (lowercaseFilename.find(lowercaseSearch) != std::string::npos) {
						m_filteredContent.push_back(&content);
					}
				}
			}
		} else {
			m_filteredContent.reserve(m_directoryMap[m_currentPath].size());
			for (auto& content : m_directoryMap[m_currentPath]) {
				m_filteredContent.push_back(&content);
			}
		}

		std::sort(m_filteredContent.begin(), m_filteredContent.end(),
			[](const DirectoryContent* a, const DirectoryContent* b) {
			if (a->m_isDirectory && !b->m_isDirectory) return true;
			if (!a->m_isDirectory && b->m_isDirectory) return false;
			return a->m_path.filename() < a->m_path.filename();
		});
	}

	void AssetBrowserPanel::renderDirectoryTree(const std::filesystem::path& path) {
		ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAllColumns |
			ImGuiTreeNodeFlags_FramePadding;

		if (path == ProjectManager::getCurrentProject().getAssetsFolder()) {
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

		bool rootNodeOpen = ImGui::TreeNodeEx(FileSystem::pathToString(path.filename()).c_str(), rootFlags);

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
		float padding = DPI::getScale(10.0f);
		float cellSize = DPI::getScale(m_cellSize);
		int columnsCount = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / (cellSize + padding)));

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(padding, padding));
		ImGui::Columns(columnsCount, "AssetColumns", false);

		uint32_t fileIndex = 0;
		for (int i = 0; i < m_filteredContent.size(); i++) {
			auto* file = m_filteredContent[i];
			const int maxNameLines = 2;
			float nameAssetTypePadding = DPI::getScale(5.0f);
			float bottomPadding = DPI::getScale(5.0f);

			ImGui::PushID(file);
			ImGui::BeginGroup();
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);

			ImDrawList* drawList = ImGui::GetWindowDrawList();
			ImVec2 pos = ImGui::GetCursorScreenPos();

			ImVec2 cardSize = ImVec2(cellSize, cellSize + ImGui::GetTextLineHeight() * (maxNameLines + 1) + nameAssetTypePadding + bottomPadding);

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

			m_idToPath[file->m_imGuiId] = file->m_path;

			ImGui::SetNextItemSelectionUserData(fileIndex);

			ImGui::PushStyleColor(ImGuiCol_HeaderHovered, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_HeaderActive, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_Header, Color::transparent());
			ImGui::PushStyleColor(ImGuiCol_NavHighlight, Color::transparent());

			bool isSelected = m_imGuiSelection.Contains(file->m_imGuiId);
			ImGui::Selectable("##AssetCard", isSelected, 0, cardSize);

			ImGui::PopStyleColor(4);


			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
				auto& selectedAssets = getSelectedAssets();
				Vector<UUID> selectedAssetUUIDs;
				selectedAssetUUIDs.reserve(selectedAssets.size() + 1);
				for (const auto& selectedAsset : selectedAssets) {
					AssetMetadata* selectedAssetMetadata = AssetRegistry::getAssetMetadata(selectedAsset);
					AssetRegistry::streamAsset(selectedAssetMetadata);
					selectedAssetUUIDs.pushBack(selectedAssetMetadata->getUUID());
				}

				if (!selectedAssetUUIDs.contains(file->m_metadata->getUUID())) {
					selectedAssetUUIDs.pushBack(file->m_metadata->getUUID());
					AssetRegistry::streamAsset(file->m_metadata);
				}

				const size_t payloadSize = selectedAssetUUIDs.size() * sizeof(UUID);
				ImGui::SetDragDropPayload(DRAG_DROP_ASSET, selectedAssetUUIDs.data(), payloadSize);

				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Open in Explorer")) {
					FileSystem::openFolderAndSelectItem(file->m_path);
				}
				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (file->m_isDirectory) {
					setCurrentPath(file->m_path);
				}
			}

			ImGui::SetCursorScreenPos(pos);

			uint32_t icon = file->m_isDirectory ? file->m_isEmpty ? m_folderEmptyIcon->handle() : m_folderIcon->handle() : s_assetTypeIcons[file->m_metadata->getType()]->handle();
			ImGui::Image((void*)(uint64_t)icon, ImVec2(cellSize, cellSize));

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
			ImGui::SetCursorScreenPos(ImVec2(pos.x + 4, pos.y + cellSize));

			// Filename
			float lineHeight = ImGui::GetTextLineHeight();
			float maxHeight = lineHeight * maxNameLines;
			ImVec2 textRegionSize = ImVec2(cardSize.x - 8, maxHeight);
			ImGui::BeginChild("TextRegion", textRegionSize, false, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs);

			ImGuiManager::pushFont(INTER_BOLD);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + cardSize.x - 8);
			ImGui::Text("%s", file->m_path.stem().u8string().c_str());
			ImGui::PopTextWrapPos();
			ImGuiManager::popFont();

			ImGui::EndChild();

			// Asset type
			const char* assetTypeText = assetTypeToString(file->m_metadata->getType(), true);
			ImVec2 textSize = ImGui::CalcTextSize(assetTypeText);

			ImVec2 cellMin = pos;
			ImVec2 cellMax = ImVec2(
				pos.x + cardSize.x,
				pos.y + cardSize.y
			);

			ImVec2 assetTypePos = ImVec2(
				pos.x + cardSize.x - textSize.x - DPI::getScale(10.0f),
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
		//if (m_updateDirectoryContentsNextFrame) {
		updateDirectoryContents();
		m_updateDirectoryContentsNextFrame = false;
		//}

		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav)) {
			static float splitRatio = 0.25f;

			ImGui::BeginGroup();
			{
				// Left Panel: Directory Tree
				ImGui::BeginChild("Directories", ImVec2(ImGui::GetContentRegionAvail().x * splitRatio, -1), ImGuiChildFlags_Border);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				if (ProjectManager::hasOpenProject()) renderDirectoryTree(ProjectManager::getCurrentProject().getAssetsFolder());
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

						std::filesystem::path displayPath = m_currentPath.lexically_relative(ProjectManager::getCurrentProject().getProjectFolder());
						displayPath = displayPath.lexically_normal();

						std::filesystem::path currentBreadcrumb = ProjectManager::getCurrentProject().getProjectFolder();

						uint32_t index = 0;
						size_t breadcrumbCount = std::distance(displayPath.begin(), displayPath.end());
						for (const auto& part : displayPath) {
							ImGui::SameLine(0, 2.0f);
							if (index != 0) {
								ImGui::Text(">");
								ImGui::SameLine(0, 2.0f);
							}

							currentBreadcrumb /= part;
							std::string partString = FileSystem::pathToString(part);
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
						if (ImGui::InputTextWithHint(UNIQUE_IMGUI_LABEL(), ICON_FA_SEARCH " Search...", m_searchBuffer, 256, ImGuiInputTextFlags_EscapeClearsAll)) {
							updateFilter();
						}

						ImGui::TableSetColumnIndex(3);
						if (ImGui::Button(ICON_FA_TIMES)) {
							memset(m_searchBuffer, 0, sizeof(m_searchBuffer));
							updateFilter();
						}

						ImGui::EndTable();
					}


					ImGui::BeginChild("##AssetGrid", ImVec2(0, -1), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
					ImGuiMultiSelectFlags flags = ImGuiMultiSelectFlags_ClearOnClickVoid | ImGuiMultiSelectFlags_SelectOnClickRelease;

					ImGuiMultiSelectIO* ms_io = ImGui::BeginMultiSelect(flags, m_imGuiSelection.Size, (uint32_t)m_filteredContent.size());
					bool shouldUpdateSelected = ms_io->Requests.size() > 0;

					m_imGuiSelection.ApplyRequests(ms_io);

					if (ProjectManager::hasOpenProject()) renderAssetGrid();

					ms_io = ImGui::EndMultiSelect();

					shouldUpdateSelected |= ms_io->Requests.size() > 0;

					m_imGuiSelection.ApplyRequests(ms_io);

					ImGui::EndChild();

					if (shouldUpdateSelected) {
						updateSelectedAssets();
					}
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