#pragma once
#include "project.h"

namespace emerald {
	class ProjectManager {
	public:
		static Project& getCurrentProject();
		static bool hasOpenProject();
		static bool createProject(const std::filesystem::path& folder);
		static bool openProject(const std::filesystem::path& projectFile);
		static bool openLastProject();
		static void closeProject();

		// Recent projects
		static void loadRecentProjects();
		static void saveRecentProjects();
		static const std::vector<std::filesystem::path>& getRecentProjects();
		static void clearRecentProjects();

		// Show a popup for selecting or creating a project
		static void showProjectPopup(bool open);

		// These might open file dialogs, then call createProject/openProject
		static void newProjectDialog();
		static void openProjectDialog();

	private:
		static inline Project s_currentProject;
		static inline bool s_hasOpenProject = false;

		static inline std::vector<std::filesystem::path> s_recentProjects;
		static inline const size_t s_maxRecentProjects = 10;

		// Helper to insert at front of s_recentProjects
		static void addToRecentProjects(const std::filesystem::path& projectFile);
	};
}