#pragma once
#include "editorProjectOpenedEvent.h"
#include <stack>
#include "engine/input/mouse.h"
#include "imgui/imgui.h"
#include <unordered_set>
#include "graphics/textures/texture.h"

namespace emerald {
	struct PathHash {
		std::size_t operator()(const std::filesystem::path& path) const {
			// Use standard library hash on the path's string representation
			return std::hash<std::string>{}(path.string());
		}
	};

	class AssetBrowserPanel {
	public:
		std::unordered_set<std::filesystem::path, PathHash> m_selectedAssets;
		std::filesystem::path m_lastSelectedAsset;

		AssetBrowserPanel();
		void draw();

		void selectAsset(const std::filesystem::path& asset) {
			m_selectedAssets.clear();
			m_selectedAssets.insert(asset);
		}

		void clearAssetSelection() {
			m_selectedAssets.clear();
		}

		void toggleAssetSelection(const std::filesystem::path& asset) {
			auto it = m_selectedAssets.find(asset);
			if (it != m_selectedAssets.end()) {
				m_selectedAssets.erase(it);
			} else {
				m_selectedAssets.insert(asset);
			}
		}

		bool isAssetSelected(const std::filesystem::path& asset) const {
			return m_selectedAssets.find(asset) != m_selectedAssets.end();
		}

		// Clear all selections
		void clearSelections() {
			m_selectedAssets.clear();
		}

		// Get all selected assets
		const std::unordered_set<std::filesystem::path, PathHash>& getSelectedAssets() const {
			return m_selectedAssets;
		}

		// Multi-select with range (e.g., shift+click)
		void selectAssetsInRange(const std::vector<std::filesystem::path>& allAssets,
			const std::filesystem::path& startAsset,
			const std::filesystem::path& endAsset) {
			auto start = std::find(allAssets.begin(), allAssets.end(), startAsset);
			auto end = std::find(allAssets.begin(), allAssets.end(), endAsset);

			if (start == allAssets.end() || end == allAssets.end()) return;

			// Ensure correct order
			if (start > end) std::swap(start, end);

			m_selectedAssets.clear();
			for (auto it = start; it <= end; ++it) {
				m_selectedAssets.insert(*it);
			}
		}

	private:
		bool m_changedPath = false; //This is a workaround, because the focus is not set correctly when navigating back and forward
		bool m_windowHovered;
		bool m_assetGridHovered;
		std::filesystem::path m_currentPath;
		std::stack<std::filesystem::path> m_forwardStack;
		ImGuiSelectionBasicStorage m_selectionStorage;
		char m_searchBuffer[256] = { 0 };
		float m_cellSize = 120.0f;

		Ref<Texture> m_folderIcon;
		Ref<Texture> m_folderEmptyIcon;

		void navigateBack();
		void navigateForward();
		void setCurrentPath(const std::filesystem::path& newPath);


		void onProjectOpened(EditorProjectOpenedEvent& e);
		void onMouseScrollEvent(MouseScrollEvent& e);
		void onMouseButtonEvent(MouseButtonEvent& e);

		std::vector<std::filesystem::path> getFilteredDirectoryContents();
		void drawBreadcrumbPart(const std::string& label, uint32_t index, const std::filesystem::path& breadcrumbPath, bool isLast);
		void renderAssetGrid();
		void renderDirectoryTree(const std::filesystem::path& rootPath);
	};
}