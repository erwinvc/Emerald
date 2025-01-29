#include "eepch.h"
#include "projectManager.h"
#include "ui/imguiManager.h"
#include "engine/events/eventSystem.h"
#include "editor/events/editorProjectOpenedEvent.h"
#include "utils/system/fileSystem.h"

namespace emerald {
	Project& ProjectManager::getCurrentProject() {
		return s_currentProject;
	}

	bool ProjectManager::hasOpenProject() {
		return s_hasOpenProject && s_currentProject.isOpen();
	}

	bool ProjectManager::createProject(const std::filesystem::path& folder) {
		if (s_currentProject.create(folder)) {
			s_hasOpenProject = true;
			addToRecentProjects(s_currentProject.getProjectFile());
			EventSystem::dispatch<EditorProjectOpenedEvent>(s_currentProject.getProjectFolder(), s_currentProject.getData().lastScenePath, true);
			return true;
		}
		return false;
	}

	bool ProjectManager::openProject(const std::filesystem::path& projectFile) {
		if (s_currentProject.open(projectFile)) {
			s_hasOpenProject = true;
			addToRecentProjects(s_currentProject.getProjectFile());
			EventSystem::dispatch<EditorProjectOpenedEvent>(s_currentProject.getProjectFolder(), s_currentProject.getData().lastScenePath, true);
			return true;
		}
		return false;
	}

	bool ProjectManager::openLastProject() {
		if (!s_recentProjects.empty()) {
			return openProject(s_recentProjects.front());
		}
		return false;
	}

	void ProjectManager::closeProject() {
		s_currentProject = Project{};
		s_hasOpenProject = false;
	}

	void ProjectManager::addToRecentProjects(const std::filesystem::path& projectFile) {
		auto it = std::find(s_recentProjects.begin(), s_recentProjects.end(), projectFile);
		if (it != s_recentProjects.end()) {
			s_recentProjects.erase(it);
		}
		s_recentProjects.insert(s_recentProjects.begin(), projectFile);

		if (s_recentProjects.size() > s_maxRecentProjects) {
			s_recentProjects.resize(s_maxRecentProjects);
		}
		saveRecentProjects();
	}

	void ProjectManager::loadRecentProjects() {
		s_recentProjects.clear();
		try {
			auto appDataPath = FileSystem::getAppDataPath();
			auto recentProjectsPath = appDataPath / "Emerald" / "recent_projects.json";
			if (!std::filesystem::exists(recentProjectsPath)) return;

			auto j = jsonUtils::readFromFile(recentProjectsPath);
			s_recentProjects = j["projects"].get<std::vector<std::filesystem::path>>();
		} catch (...) {

		}
	}

	void ProjectManager::saveRecentProjects() {
		nlohmann::json j;
		j["projects"] = s_recentProjects;

		auto appDataPath = FileSystem::getAppDataPath();
		auto recentProjectsPath = appDataPath / "Emerald" / "recent_projects.json";
		std::filesystem::create_directories(recentProjectsPath.parent_path());
		jsonUtils::saveToFile(j, recentProjectsPath);
	}

	const std::vector<std::filesystem::path>& ProjectManager::getRecentProjects() {
		return s_recentProjects;
	}

	void ProjectManager::clearRecentProjects() {
		s_recentProjects.clear();
		saveRecentProjects();
	}

	void ProjectManager::showProjectPopup(bool open) {
		if (open) {
			ImGui::OpenPopup("Select Project");
		}
		if (ImGui::BeginPopupModal("Select Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::Button("Create New Project")) {
				std::string newProjectPath = "path_to_new_project"; 
				createProject(newProjectPath);
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::Button("Open Existing Project")) {
				std::string existingProjectPath = "path_to_existing_project"; 
				openProject(existingProjectPath);
				ImGui::CloseCurrentPopup();
			}
			ImGui::Separator();
			ImGui::Text("Recent Projects:");
			for (const auto& proj : s_recentProjects) {
				if (ImGui::Button(proj.string().c_str())) {
					openProject(proj);
					ImGui::CloseCurrentPopup();
				}
			}
			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void ProjectManager::newProjectDialog() {
		auto path = FileSystem::openFolderDialog(L"Choose location for new project");
		if (!path.empty()) {
			createProject(path);
		}
	}

	void ProjectManager::openProjectDialog() {
		auto path = FileSystem::openFileDialog({ {L"Emerald Project Files", L"*.eep"} });
		if (!path.empty()) {
			openProject(path);
		}
	}
}