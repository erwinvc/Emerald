#include "eepch.h"
#include "assetBrowserPanel.h"
#include "ui/imguiManager.h"
#include "project.h"
#include "engine/events/eventSystem.h"
#include "ui/iconsFontAwesome.h"

namespace emerald {
	AssetBrowserPanel::AssetBrowserPanel() {
		EventSystem::subscribe<EditorProjectOpenedEvent>(&AssetBrowserPanel::onProjectOpened, this);
	}

	void AssetBrowserPanel::onProjectOpened(EditorProjectOpenedEvent& e) {
		if (e.isValid()) {
			currentPath = Project::GetAssetsPath();
		}
	}

	std::vector<std::filesystem::path> AssetBrowserPanel::getFilteredDirectoryContents() {
		std::vector<std::filesystem::path> filteredContents;
		if (!Project::isProjectOpen() || !std::filesystem::is_directory(currentPath)) return filteredContents;

		for (const auto& entry : std::filesystem::directory_iterator(currentPath)) {
			// Skip hidden files if not enabled
			if (entry.path().filename().string().starts_with('.'))
				continue;

			// Apply search filter
			if (strlen(searchBuffer) > 0) {
				std::string lowercaseSearch = searchBuffer;
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

	void AssetBrowserPanel::renderDirectoryTree(const std::filesystem::path& path) {
		ImGuiTreeNodeFlags rootFlags = ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_SpanFullWidth |
			ImGuiTreeNodeFlags_NavLeftJumpsBackHere |
			ImGuiTreeNodeFlags_AllowItemOverlap |
			ImGuiTreeNodeFlags_SpanAllColumns |
			ImGuiTreeNodeFlags_FramePadding;

		if (path == currentPath) rootFlags |= ImGuiTreeNodeFlags_Selected;

		bool hasSubdirectories = false;
		for (const auto& entry : std::filesystem::directory_iterator(path)) {
			if (std::filesystem::is_directory(entry)) {
				hasSubdirectories = true;
				break;
			}
		}

		if (!hasSubdirectories) rootFlags |= ImGuiTreeNodeFlags_Leaf;

		bool rootNodeOpen = ImGui::TreeNodeEx(path.filename().string().c_str(), rootFlags);

		if (ImGui::IsItemClicked()) {
			currentPath = path;
		}

		if (ImGui::IsItemFocused()) {
			currentPath = path; 
		}

		if (rootNodeOpen) {
			// Render subdirectories
			for (const auto& entry : std::filesystem::directory_iterator(path)) {
				if (!std::filesystem::is_directory(entry)) continue;

				bool isCurrentPath = entry.path() == currentPath;

				renderDirectoryTree(entry.path());
			}

			ImGui::TreePop();
		}
	}

	void AssetBrowserPanel::renderAssetGrid() {
		auto contents = getFilteredDirectoryContents();

		// Grid layout
		float cellSize = 100.0f;
		int columnsCount = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / cellSize));

		ImGui::Columns(columnsCount, "AssetColumns", false);

		for (const auto& asset : contents) {
			ImGui::BeginGroup();

			// Placeholder for thumbnail (you'd replace this with actual thumbnail rendering)
			ImGui::Button(asset.filename().string().c_str(), ImVec2(cellSize, cellSize));

			// Double-click to open directory or file
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
				if (std::filesystem::is_directory(asset)) {
					currentPath = asset;
				} else {
					// Implement file opening logic here
				}
			}

			ImGui::TextWrapped("%s", asset.filename().string().c_str());
			ImGui::EndGroup();

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
	}

	void AssetBrowserPanel::draw() {
		ImGui::ApplyNodeFlagsToNextWindow(ImGuiDockNodeFlags_NoWindowMenuButton);
		if (ImGui::Begin("Assets", nullptr, ImGuiWindowFlags_NoNav)) {
			ImGui::BeginGroup();

			if (ImGui::BeginTable(UNIQUE_IMGUI_LABEL(), 2, ImGuiTableFlags_None)) {
				ImGui::TableSetupColumn("InputText", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Button", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::SetNextItemWidth(-FLT_MIN);
				ImGui::InputTextWithHint(UNIQUE_IMGUI_LABEL(), ICON_FA_SEARCH " Search...", searchBuffer, 256, ImGuiInputTextFlags_EscapeClearsAll);

				ImGui::TableSetColumnIndex(1);
				if (ImGui::Button(ICON_FA_TIMES)) {
					memset(searchBuffer, 0, sizeof(searchBuffer));
				}

				ImGui::EndTable();
			}

			ImGui::EndGroup();

			ImGui::BeginGroup();
			std::filesystem::path displayPath = currentPath.lexically_relative(Project::GetProjectPath());
			displayPath = displayPath.lexically_normal();
			std::string fullPath = "Assets";

			ImGui::Text("Path: ");
			ImGui::SameLine();

			std::filesystem::path currentBreadcrumb = Project::GetProjectPath();
			int id = 0;
			for (const auto& part : displayPath) {
				fullPath += " > " + part.string();

				ImGui::SameLine(0, 2);
				if (ImGui::Button((part.string() + "##" + std::to_string(id++)).c_str())) {
					currentPath = currentBreadcrumb;
				}
				currentBreadcrumb /= part;

			}
			ImGui::EndGroup();

			// Split view
			static float splitRatio = 0.25f;
			ImGui::BeginGroup();
			{
				// Left Panel: Directory Tree
				ImGui::BeginChild("Directories", ImVec2(ImGui::GetContentRegionAvail().x * splitRatio, -1), true);

				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				if (Project::isProjectOpen()) renderDirectoryTree(Project::GetAssetsPath());
				ImGui::PopStyleVar(2);
				ImGui::EndChild();

				ImGui::SameLine();

				// Right Panel: Asset Grid
				ImGui::BeginChild("Assets", ImVec2(0, -1), true);
				if (Project::isProjectOpen()) renderAssetGrid();
				ImGui::EndChild();
			}
			ImGui::EndGroup();
		}
		ImGui::End();
	}
}