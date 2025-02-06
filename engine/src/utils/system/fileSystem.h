#pragma once

namespace emerald {
	class FileSystem {
	public:
		struct FilterSpec {
			LPCWSTR name;
			LPCWSTR spec;
		};

		static std::filesystem::path openFileDialog(const std::vector<FilterSpec>& filters);
		static std::filesystem::path saveFileDialog(FilterSpec filter);
		static std::filesystem::path openFolderDialog(LPCWSTR title);

		static std::string readFile(const std::string& path);

		static bool doesFileExist(const std::filesystem::path& path);

		static void saveJsonToFile(const nlohmann::json& jsonOb, const std::string& name);

		static nlohmann::json loadJsonFromFile(const std::string& name);

		static void saveStringToFile(const std::string& str, const std::string& name);
		static void createFile(const std::string& file);

		static bool createFileIfDoesntExist(const std::string& file);

		static std::string getShortFilename(const std::string& filename);
		static void copyFile(const std::string& source, const std::string& dest);

		static std::filesystem::path getAppDataPath();

		static void openFolderAndSelectItem(const std::filesystem::path& path);

		static inline std::string pathToString(const std::filesystem::path& p) {
			auto u8str = p.u8string();
			return std::string(u8str.begin(), u8str.end());
		}
	};
}