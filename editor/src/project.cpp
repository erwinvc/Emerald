#include "eepch.h"
#include "project.h"
#include <filesystem>
#include "ui/imguiManager.h"
#include "core/common/engineError.h"
#include "utils/system/fileSystem.h"
#include "editor.h"
#include "engine/events/eventSystem.h"
#include "editorProjectOpenedEvent.h"

namespace emerald {
	static std::filesystem::path s_projectPath;
	static std::vector<std::filesystem::path> s_recentProjects;
	static const size_t s_maxRecentProjects = 10;
	static std::function<void()> s_onProjectLoaded;

	void Project::createProject(const std::filesystem::path& path) {
		try {
			if (!std::filesystem::exists(path)) {
				EngineError::raise(Severity::WARN, "Failed creating project", "Specified path does not exist.");
				return;
			}

			auto projectFilePath = path / "project.eep";
			if (std::filesystem::exists(projectFilePath)) {
				EngineError::raise(Severity::WARN, "Failed creating project", "A project file already exists at the specified location.");
				return;
			}

			auto assetsFolderPath = path / "assets";
			if (!std::filesystem::exists(assetsFolderPath)) {
				std::filesystem::create_directory(assetsFolderPath);
			}

			std::ofstream projectFile(projectFilePath);

			if (projectFile.is_open()) {
				nlohmann::json j;
				j["version"] = "0.0.1";
				projectFile << j.dump(4);
				projectFile.close();
			} else {
				EngineError::raise(Severity::WARN, "Failed creating project", std::format("Failed to create project file at: {}", projectFilePath.string()));
				return;
			}

			s_projectPath = path;
			addToRecentProjects(path);

			EventSystem::dispatch<EditorProjectOpenedEvent>(true);
		} catch (const std::filesystem::filesystem_error& e) {
			EngineError::raise(Severity::WARN, "Failed creating project", std::format("Filesystem error: {}", e.what()));
		}
	}

	void Project::openProject(const std::filesystem::path& path) {
		s_projectPath = path;

		EventSystem::dispatch<EditorProjectOpenedEvent>(true);

		addToRecentProjects(path);
	}

	bool Project::isProjectOpen() { return !s_projectPath.empty(); }

	const std::filesystem::path& Project::GetProjectPath() {
		return s_projectPath;
	}
	std::filesystem::path Project::GetAssetsPath() {
		return s_projectPath / "assets";
	}

	const std::string Project::getProjectFolderName() {
		return Project::GetProjectPath().parent_path().filename().string();
	}

	void Project::addToRecentProjects(const std::filesystem::path& path) {
		s_recentProjects.erase(std::remove(s_recentProjects.begin(), s_recentProjects.end(), path), s_recentProjects.end());
		s_recentProjects.insert(s_recentProjects.begin(), path);
		if (s_recentProjects.size() > s_maxRecentProjects) {
			s_recentProjects.resize(s_maxRecentProjects);
		}

		saveRecentProjects();
	}


	void Project::saveRecentProjects() {
		std::ofstream out("recent_projects.txt");
		for (const auto& project : s_recentProjects) {
			out << project << "\n";
		}
	}

	void Project::loadRecentProjects() {
		s_recentProjects.clear();
		std::ifstream in("recent_projects.txt");
		std::string line;
		while (std::getline(in, line)) {
			if (!line.empty()) {
				s_recentProjects.push_back(line);
			}
		}
	}

	void Project::showProjectPopup(bool open) {
		if (open) {
			ImGui::OpenPopup("Select Project");
		}
		if (ImGui::BeginPopupModal("Select Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
			if (ImGui::Button("Create New Project")) {
				std::string newProjectPath = "path_to_new_project"; // Obtain from file dialog or user input
				createProject(newProjectPath);
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::Button("Open Existing Project")) {
				std::string existingProjectPath = "path_to_existing_project"; // Obtain from file dialog
				openProject(existingProjectPath);
				ImGui::CloseCurrentPopup();
			}

			ImGui::Separator();
			ImGui::Text("Recent Projects:");

			for (const auto& project : s_recentProjects) {
				if (ImGui::Button(project.string().c_str())) {
					openProject(project);
					ImGui::CloseCurrentPopup();
				}
			}

			if (ImGui::Button("Cancel")) {
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void Project::newProjectDialog() {
		const std::filesystem::path path = FileSystem::openFolderDialog(L"Choose location for new project");
		if (path.empty()) return;
		createProject(path);
	}

	void Project::openProjectDialog() {
		const std::filesystem::path path = FileSystem::openFileDialog({ {L"Emerald Project Files", L"*.eep"} });
		if (path.empty()) return;
		openProject(path);
	}

}