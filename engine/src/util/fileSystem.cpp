#include "eepch.h"
#include "fileSystem.h"
#include <format>
#include <chrono>

namespace emerald {
	namespace FileSystem {
		std::string readFile(const std::string& path) {
			std::ifstream stream(path);
			std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
			stream.close();
			return str;
		}

		bool doesFileExist(const std::string& path) {
			struct stat buffer;
			return (stat(path.c_str(), &buffer) == 0);
		}

		void saveJsonToFile(const nlohmann::json& jsonOb, const std::string& name) {
			std::string file = std::format("{}.json", name);
			std::ofstream i(file);
			i << std::setw(4) << jsonOb;
			i.close();
		}

		nlohmann::json loadJsonFromFile(const std::string& name) {
			std::string file = std::format("{}.json", name);
			std::ifstream i(file);
			if (i.fail()) {
				Log::error("Failed to open json file");
				return NULL;
			}
			nlohmann::json jsonOb = nlohmann::json::parse(i);
			i.close();
			return jsonOb;
		}

		void saveStringToFile(const std::string& str, const std::string& name) {
			std::string file = std::format("{}.txt", name);
			std::ofstream i(file);
			i << str;
			i.close();
		}

		void createFile(const std::string& file) {
			std::ofstream i(file);
			i.close();
		}

		bool createFileIfDoesntExist(const std::string& file) {
			bool exists = doesFileExist(file);
			if (!exists) createFile(file);
			return !exists;
		}

		std::string getShortFilename(const std::string& filename) {
			const char* lastSlash = strrchr(filename.c_str(), '/');
			if (lastSlash == nullptr) {
				lastSlash = strrchr(filename.c_str(), '\\');
			}
			std::string shortFilename = lastSlash != nullptr ? lastSlash + 1 : filename;
			return shortFilename;
		}

		void copyFile(const std::string& source, const std::string& dest) {
			if (doesFileExist(source)) {
				std::ifstream src(source.c_str(), std::ios::binary);
				std::ofstream dst(dest.c_str(), std::ios::binary);
				dst << src.rdbuf();
				src.close();
				dst.close();
			}
		}
	}
}