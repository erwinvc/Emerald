#include "stdafx.h"

void FileSystem::SaveJsonToFile(const nlohmann::json& jsonOb, const String& name) {
	String file = Format("%s.json", name.c_str());
	ofstream i(file);
	i << setw(4) << jsonOb;
	i.close();
}

nlohmann::json FileSystem::LoadJsonFromFile(const String& name) {
	String file = Format("%s.json", name.c_str());
	ifstream i(file);
	if (i.fail()) LOG_ERROR("[~bJson~x] Failed to open json file");
	nlohmann::json jsonOb = nlohmann::json::parse(i);
	i.close();
	return jsonOb;
}

void FileSystem::SaveStringToFile(const String& str, const String& name) {
	String file = Format("%s.txt", name.c_str());
	ofstream i(file);
	i << str;
	i.close();
}

void FileSystem::CreateFile(const String& file) {
	ofstream i(file);
	i.close();
}

bool FileSystem::CreateFileIfDoesntExist(const String& file) {
	bool exists = DoesFileExist(file);
	if (!exists) CreateFile(file);
	return !exists;
}

String_t FileSystem::GetShortFilename(String_t filename) {
	const char* lastSlash = strrchr(filename, '/');
	if (lastSlash == nullptr) {
		lastSlash = strrchr(filename, '\\');
	}
	String_t shortFilename = lastSlash != nullptr ? lastSlash + 1 : filename;
	return shortFilename;
}

void FileSystem::CopyFile(const String& source, const String& dest) {
	if (DoesFileExist(source)) {
		std::ifstream src(source.c_str(), std::ios::binary);
		std::ofstream dst(dest.c_str(), std::ios::binary);
		dst << src.rdbuf();
		dst.close();
	}
}