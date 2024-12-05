#pragma once
#include "editorProjectOpenedEvent.h"

namespace emerald {
	class AssetBrowserPanel {
	public:
		AssetBrowserPanel();
		void draw();
	private:
		std::filesystem::path currentPath;
		char searchBuffer[256] = { 0 };

		void onProjectOpened(EditorProjectOpenedEvent& e);
		std::vector<std::filesystem::path> getFilteredDirectoryContents();
		void renderAssetGrid();
		void renderDirectoryTree(const std::filesystem::path& rootPath);
	};
}