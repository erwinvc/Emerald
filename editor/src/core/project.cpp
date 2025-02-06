#include "eepch.h"
#include "project.h"
#include <filesystem>
#include "core/common/engineError.h"

namespace emerald {
	bool Project::create(const std::filesystem::path& projectFolder) {
		try {
			if (!std::filesystem::exists(projectFolder)) {
				EngineError::raise(Severity::WARN, "Failed to create project",
					"Specified folder does not exist.");
				return false;
			}

			m_projectFolder = projectFolder;
			m_projectFile = m_projectFolder / "project.eep";

			if (std::filesystem::exists(m_projectFile)) {
				EngineError::raise(Severity::WARN, "Failed to create project",
					"A project.eep already exists in that folder.");
				return false;
			}

			auto assetsFolder = m_projectFolder / "assets";
			if (!std::filesystem::exists(assetsFolder)) {
				std::filesystem::create_directory(assetsFolder);
			}

			m_data = ProjectData{};

			return save();
		} catch (const std::filesystem::filesystem_error& e) {
			EngineError::raise(Severity::WARN, "Failed creating project", std::string("Filesystem error: ") + e.what());
			return false;
		}
	}

	bool Project::open(const std::filesystem::path& projectFile) {
		try {
			if (!std::filesystem::exists(projectFile)) {
				EngineError::raise(Severity::WARN, "Failed opening project", "Specified file does not exist.");
				return false;
			}

			m_projectFile = projectFile;
			m_projectFolder = projectFile.parent_path();

			return load(); // read the .eep data
		} catch (const std::filesystem::filesystem_error& e) {
			EngineError::raise(Severity::WARN, "Failed opening project", std::string("Filesystem error: ") + e.what());
			return false;
		}
	}

	bool Project::save() {
		if (m_projectFile.empty()) {
			return false;
		}
		try {
			jsonUtils::saveToFile(m_data, m_projectFile);
			return true;
		} catch (std::exception& ex) {
			std::u8string s = m_projectFile.u8string();
			EngineError::raise(Severity::WARN, "Failed saving project", std::format("Could not write: {} because {}", s, ex.what()));
			return false;
		}
	}

	bool Project::load() {
		if (m_projectFile.empty()) {
			return false;
		}
		try {
			m_data = jsonUtils::readFromFile(m_projectFile);
			return true;
		} catch (std::exception& ex) {
			std::u8string s = m_projectFile.u8string();
			EngineError::raise(Severity::WARN, "Failed loading project", std::format("Could not read: {} because {}", s, ex.what()));
			return false;
		}
	}

	std::filesystem::path Project::getAssetsFolder() const {
		return m_projectFolder / "assets";
	}
}