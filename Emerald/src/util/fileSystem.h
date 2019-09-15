#pragma once

class FileSystem {
public:
	static String ReadFile(String path) {
		ifstream stream(path);
		string str((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
		stream.close();
		return str;
	}

	static bool DoesFileExist(const String& path) {
		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}

	static void SaveJsonToFile(const nlohmann::json& jsonOb, const String& name);

	static nlohmann::json LoadJsonFromFile(const String& name);

	static void SaveStringToFile(const String& str, const String& name);
	static void CreateFile(const String& file);

	static bool CreateFileIfDoesntExist(const String& file) {
		bool exists = DoesFileExist(file);
		if (!exists) CreateFile(file);
		return !exists;
	}

	static String_t GetShortFilename(const char* filename) {
		const char* lastSlash = strrchr(filename, '/');
		if (lastSlash == nullptr) {
			lastSlash = strrchr(filename, '\\');
		}
		String_t shortFilename = lastSlash != nullptr ? lastSlash + 1 : filename;
		return shortFilename;
	}

};