#pragma once

namespace emerald {
	namespace FileSystem {
		std::string readFile(const std::string& path);

		bool doesFileExist(const std::string& path);

		void saveJsonToFile(const nlohmann::json& jsonOb, const std::string& name);

		nlohmann::json loadJsonFromFile(const std::string& name);

		void saveStringToFile(const std::string& str, const std::string& name);
		void createFile(const std::string& file);

		bool createFileIfDoesntExist(const std::string& file);

		std::string getShortFilename(const std::string& filename);
		void copyFile(const std::string& source, const std::string& dest);
	}
}