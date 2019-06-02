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
		ifstream i(path.c_str());
		return i.good();
		//DWORD result = GetFileAttributesA(path.c_str());
		//return !(result == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND);
	}
};