#pragma once
#include "editorProjectOpenedEvent.h"
#include <stack>
#include "engine/input/mouse.h"
#include "imgui/imgui.h"
#include <unordered_set>
#include "graphics/textures/texture.h"

namespace emerald {
	class AssetBrowserPanel {
	public:

		AssetBrowserPanel();
		void draw();

		const std::vector<std::filesystem::path>& getSelectedAssets() const {
			return m_selectedAssets;
		}

		struct AssetEntry {
			std::filesystem::path m_path;
			ImGuiID m_imGuiID;
			bool m_isDirectory;
			AssetMetadata* m_metadata;
		};;

	private:
		std::filesystem::path m_lastSelectedAsset;
		std::vector<AssetEntry> filteredContents;
		ImGuiSelectionBasicStorage m_imGuiSelection;
		bool m_windowHovered;
		bool m_assetGridHovered;
		std::filesystem::path m_currentPath;
		std::stack<std::filesystem::path> m_forwardStack;
		ImGuiSelectionBasicStorage m_selectionStorage;
		std::vector<std::filesystem::path> m_selectedAssets;
		std::unordered_map<ImGuiID, std::filesystem::path> m_idToPath;
		char m_searchBuffer[256] = { 0 };
		float m_cellSize = 120.0f;

		Ref<Texture> m_meshIcon;
		Ref<Texture> m_folderIcon;
		Ref<Texture> m_folderEmptyIcon;

		void updateSelectedAssets();

		void navigateBack();
		void navigateForward();
		void setCurrentPath(const std::filesystem::path& newPath);

		void onProjectOpened(EditorProjectOpenedEvent& e);
		void onMouseButtonEvent(MouseButtonEvent& e);

		std::vector<AssetEntry> getFilteredDirectoryContents();
		void drawBreadcrumbPart(const std::string& label, uint32_t index, const std::filesystem::path& breadcrumbPath, bool isLast);
		void renderAssetGrid();
		void renderDirectoryTree(const std::filesystem::path& rootPath);
	};
}