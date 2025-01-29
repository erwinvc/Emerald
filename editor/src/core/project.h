#pragma once
#include "core/common/engineConstants.h"

namespace emerald {
	class ProjectData {
	public:
		uint32_t version = EngineConstants::VERSION_PROJECT;
		std::filesystem::path lastScenePath;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(ProjectData, version, lastScenePath)
	};

	class Project {
	public:
		Project() = default;
		~Project() = default;

		bool create(const std::filesystem::path& projectFolder);
		bool open(const std::filesystem::path& projectFile);
		bool save();
		bool load();
		bool isOpen() const { return !m_projectFile.empty(); }

		const std::filesystem::path& getProjectFolder() const { return m_projectFolder; }
		const std::filesystem::path& getProjectFile() const { return m_projectFile; }
		std::filesystem::path getAssetsFolder() const;

		const ProjectData& getData() const { return m_data; }
		ProjectData& getData() { return m_data; }

	private:
		ProjectData m_data;
		std::filesystem::path m_projectFolder; 
		std::filesystem::path m_projectFile; 
	};
}