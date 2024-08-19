#include "eepch.h"
#include "project.h"
#include "imgui.h"
#include <filesystem>

namespace emerald {
	static std::string s_currentProject;
	static std::vector<std::string> s_recentProjects;
	static const size_t s_maxRecentProjects = 10;

	void Project::createProject(const std::string& path) {
		s_currentProject = path;
		std::filesystem::create_directory(path);
		addToRecentProjects(path);
	}

	void Project::openProject(const std::string& path) {
		s_currentProject = path;
		addToRecentProjects(path);
	}

	const std::string& Project::GetCurrentProject() {
		return s_currentProject;
	}

	void Project::addToRecentProjects(const std::string& path) {
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
			Log::info("Showing project popup");
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
				if (ImGui::Button(project.c_str())) {
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
}