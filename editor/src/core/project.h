#pragma once

namespace emerald {
	class Project {
	public:
		static const std::filesystem::path& GetProjectPath();
		static std::filesystem::path GetAssetsPath();
		static const std::string getProjectFolderName();
		static void createProject(const std::filesystem::path& path);
		static void openProject(const std::filesystem::path& path);
		static bool isProjectOpen();
		static void showProjectPopup(bool open);
		static void newProjectDialog();
		static void openProjectDialog();
		static void loadRecentProjects();
		static void clearRecentProjects();

		static const std::vector<std::filesystem::path> getRecentProjects();

	private:
		static void addToRecentProjects(const std::filesystem::path& path);
		static void saveRecentProjects();
	};
}