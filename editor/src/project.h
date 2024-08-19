#pragma once

namespace emerald {
	class Project {
	public:
		static void createProject(const std::string& path);
		static void openProject(const std::string& path);
		static const std::string& GetCurrentProject();
		static void showProjectPopup(bool open);

	private:
		static void addToRecentProjects(const std::string& path);
		static void saveRecentProjects();
		static void loadRecentProjects();
	};
}