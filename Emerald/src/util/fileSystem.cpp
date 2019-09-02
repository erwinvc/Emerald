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