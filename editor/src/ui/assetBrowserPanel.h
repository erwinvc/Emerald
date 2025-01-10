#pragma once
#include "editor/events/editorProjectOpenedEvent.h"
#include <stack>
#include "engine/input/mouse.h"
#include "imgui/imgui.h"
#include "graphics/textures/texture.h"
#include "engine/events/fileChangedEvent.h"

namespace emerald {
	class AssetBrowserPanel {
	public:
		AssetBrowserPanel();
		void draw();

		const std::vector<std::filesystem::path>& getSelectedAssets() const {
			return m_selectedAssets;
		}

		struct DirectoryContent {
			std::filesystem::path m_path;
			ImGuiID m_imGuiId;
			bool m_isDirectory;
			bool m_isEmpty; // Only used for directories
			AssetMetadata* m_metadata;

			DirectoryContent(const std::filesystem::path& path, ImGuiID imGuiId, bool isDirectory, AssetMetadata* metadata)
				: m_path(path), m_imGuiId(imGuiId), m_isDirectory(isDirectory), m_isEmpty(isDirectory ? std::filesystem::is_empty(path) : true), m_metadata(metadata)
			{}
		};

	private:
		bool m_updateDirectoryContentsNextFrame = true;
		std::filesystem::path m_lastSelectedAsset;
		std::unordered_map<std::filesystem::path, std::vector<DirectoryContent>> m_directoryMap;
		std::vector<DirectoryContent*> m_filteredContent;
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
		void onFileChangedEvent(FileChangedEvent& e);

		void collectDirectoryContents(const std::filesystem::path& directoryPath);
		void updateDirectoryContents();
		void updateFilter();

		void drawBreadcrumbPart(const std::string& label, uint32_t index, const std::filesystem::path& breadcrumbPath, bool isLast);
		void renderAssetGrid();
		void renderDirectoryTree(const std::filesystem::path& rootPath);
	};
}