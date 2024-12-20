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
#include "engine/assets/streaming/streaming.h"

namespace emerald {
	static constexpr float MIN_CELL_SIZE = 50.0f;
	static constexpr float MAX_CELL_SIZE = 300.0f;

	AssetBrowserPanel::AssetBrowserPanel() {
		EventSystem::subscribe<EditorProjectOpenedEvent>(&AssetBrowserPanel::onProjectOpened, this);
		EventSystem::subscribe<MouseButtonEvent>(&AssetBrowserPanel::onMouseButtonEvent, this);
		EventSystem::subscribe<MouseScrollEvent>(&AssetBrowserPanel::onMouseScrollEvent, this);

		TextureDesc desc;
		m_folderIcon = TextureLoader(desc, "res/textures/folder.png").load();
		m_folderEmptyIcon = TextureLoader(desc, "res/textures/folderEmpty.png").load();

		Renderer::submit([instance = Ref<Texture>(m_folderIcon)]() { instance->invalidate(); });
		Renderer::submit([instance = Ref<Texture>(m_folderEmptyIcon)]() { instance->invalidate(); });
	}

	void AssetBrowserPanel::onProjectOpened(EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			m_currentPath = Project::GetAssetsPath();
			std::stack<std::filesystem::path>().swap(m_forwardStack);
		}
	}

	void AssetBrowserPanel::onMouseScrollEvent(MouseScrollEvent& e) {
		if (!m_assetGridHovered || !Keyboard::keyMod(KeyMod::CONTROL) || !Project::isProjectOpen()) return;
		m_cellSize += e.getYOffset() * 10.0f;
		int roundedCellSize = (int)m_cellSize;
		m_cellSize = std::clamp((float)roundedCellSize, MIN_CELL_SIZE, MAX_CELL_SIZE);
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
			m_changedPath = true;
			m_currentPath = m_currentPath.parent_path();
			if (m_currentPath < Project::GetAssetsPath()) {
				m_currentPath = Project::GetAssetsPath();
			}
		}
	}

	void AssetBrowserPanel::navigateForward() {
		if (!m_forwardStack.empty()) {
			m_currentPath = m_forwardStack.top();
			m_changedPath = true;
			m_forwardStack.pop();
		}
	}

	void AssetBrowserPanel::setCurrentPath(const std::filesystem::path& newPath) {
		if (newPath != m_currentPath) {
			while (!m_forwardStack.empty()) {
				m_forwardStack.pop();
			}
			m_currentPath = newPath;
		}
	}

	std::vector<std::filesystem::path> AssetBrowserPanel::getFilteredDirectoryContents() {
		std::vector<std::filesystem::path> filteredContents;
		if (!Project::isProjectOpen() || !std::filesystem::is_directory(m_currentPath)) return filteredContents;

		for (const auto& entry : std::filesystem::directory_iterator(m_currentPath)) {
			// Skip hidden files if not enabled
			if (entry.path().filename().string().starts_with('.'))
				continue;

			if (strlen(m_searchBuffer) > 0) {
				std::string lowercaseSearch = m_searchBuffer;
				std::transform(lowercaseSearch.begin(), lowercaseSearch.end(), lowercaseSearch.begin(), ::tolower);

				std::string lowercaseFilename = entry.path().filename().string();
				std::transform(lowercaseFilename.begin(), lowercaseFilename.end(), lowercaseFilename.begin(), ::tolower);

				if (lowercaseFilename.find(lowercaseSearch) == std::string::npos)
					continue;
			}

			filteredContents.push_back(entry.path());
		}

		// Sort: directories first, then alphabetically
		std::sort(filteredContents.begin(), filteredContents.end(),
			[](const std::filesystem::path& a, const std::filesystem::path& b) {
			if (std::filesystem::is_directory(a) && !std::filesystem::is_directory(b)) return true;
			if (!std::filesystem::is_directory(a) && std::filesystem::is_directory(b)) return false;
			return a.filename() < b.filename();
		});

		return filteredContents;
	}

	std::string focusedItem = "";

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
			if (m_changedPath) {
				ImGui::SetKeyboardFocusHere();
				m_changedPath = false;
			}
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

		// This is a workaround to set the focus on the recently selected item
		if (ImGui::IsItemFocused()) {
			std::string item = path.filename().string();
			if (strcmp(item.c_str(), focusedItem.c_str()) != 0) {
				focusedItem = item;
				setCurrentPath(path);
			}
		}

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

		for (const auto& assetPath : directoryContents) {
			const int maxNameLines = 2;
			const float nameAssetTypePadding = 5.0f;
			const float bottomPadding = 5.0f;

			bool isDirectory = std::filesystem::is_directory(assetPath);
			AssetMetadata* metadata = Editor->getAssetRegistry().getAssetMetadata(assetPath);
			if (!metadata && !isDirectory) continue;

			ImGui::PushID(assetPath.string().c_str());

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

			ImGui::InvisibleButton("AssetCard", cardSize);

			bool isSelected = isAssetSelected(assetPath);

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID | ImGuiDragDropFlags_SourceNoPreviewTooltip)) {
				std::vector<UUID> selectedAssets;
				for (const auto& selectedAsset : getSelectedAssets()) {
					AssetMetadata* selectedAssetMetadata = Editor->getAssetRegistry().getAssetMetadata(selectedAsset);
					Streaming::streamAsset(selectedAssetMetadata);
					selectedAssets.push_back(selectedAssetMetadata->getUUID());
				}

				const size_t payloadSize = selectedAssets.size() * sizeof(UUID);
				ImGui::SetDragDropPayload(DRAG_DROP_ASSET, selectedAssets.data(), payloadSize);

				ImGui::EndDragDropSource();
			}

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
				if (ImGui::GetIO().KeyCtrl) {
					toggleAssetSelection(assetPath);
				} else if (ImGui::GetIO().KeyShift) {
					selectAssetsInRange(directoryContents, m_lastSelectedAsset, assetPath);
				} else {
					if (!isSelected) {
						clearAssetSelection();
						selectAsset(assetPath);
					}
				}

				m_lastSelectedAsset = assetPath;
			}

			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Open in Explorer")) {
					FileSystem::openFolderAndSelectItem(assetPath);
				}
				ImGui::EndPopup();
			}

			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (isDirectory) {
					setCurrentPath(assetPath);
				}
			}

			ImVec2 thumbnailSize = ImVec2(m_cellSize, m_cellSize);

			ImGui::SetCursorScreenPos(pos);

			if (isDirectory) {
				uint32_t icon = std::filesystem::is_empty(assetPath) ? m_folderEmptyIcon->handle() : m_folderIcon->handle();
				ImGui::Image((void*)(uint64_t)icon, DPI::getScale(thumbnailSize));
			}

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
			ImGui::Text("%s", assetPath.stem().string().c_str());
			ImGui::PopTextWrapPos();
			ImGuiManager::popFont();

			ImGui::EndChild();

			// Asset type
			const char* assetTypeText = assetTypeToString(metadata->getType(), true);
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
		}

		ImGui::Columns(1);
		ImGui::PopStyleVar();  // Item spacing
	}

	// Define colors for normal, hover, and active (clicked) states
	static ImVec4 normalColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);    // White text
	static ImVec4 hoverColor = ImVec4(0.7f, 0.7f, 1.0f, 1.0f);    // Slightly bluish on hover
	static ImVec4 activeColor = ImVec4(0.5f, 0.5f, 1.0f, 1.0f);    // More intense bluish on active

	void AssetBrowserPanel::drawBreadcrumbPart(const std::string& label, uint32_t index, const std::filesystem::path& breadcrumbPath, bool isLast) {
		ImVec2 textSize = ImGui::CalcTextSize(label.c_str());
		ImVec2 pos = ImGui::GetCursorScreenPos();

		if (isLast) {
			// Draw bold text for the last breadcrumb
			//ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assumes second font is bold
			ImGuiManager::pushFont(INTER_BOLD);
			ImGui::Text("%s", label.c_str());
			ImGuiManager::popFont();
			//ImGui::PopFont();
		} else {
			// Create an invisible button for other breadcrumbs
			ImGui::PushID(label.c_str());
			ImGui::InvisibleButton(ImGuiManager::generateLabel("##invisible_btn", index), textSize);
			bool hovered = ImGui::IsItemHovered();
			bool clicked = ImGui::IsItemClicked();
			ImGui::PopID();

			// Determine text color
			ImU32 textColor = clicked ? ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
				: (hovered ? ImGui::GetColorU32(ImGuiCol_Text) : ImGui::GetColorU32(ImGuiCol_TextDisabled));

			// Draw text
			ImGui::GetWindowDrawList()->AddText(pos, textColor, label.c_str());

			// Update current path if clicked
			if (clicked) {
				setCurrentPath(breadcrumbPath);
				//m_currentPath = breadcrumbPath;
			}
		}

		// Update cursor position for next item
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
				ImGui::BeginChild("Assets", ImVec2(0, -1), true);

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
					std::string fullPath = "Assets";

					// Start from the project root path
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
						drawBreadcrumbPart(part.string(), index++, currentBreadcrumb, index == breadcrumbCount - 1);

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
				if (Project::isProjectOpen()) renderAssetGrid();
				ImGui::EndChild();
				ImGui::EndChild();
			}
			ImGui::EndGroup();
		}
		m_windowHovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
		ImGui::End();
	}
}