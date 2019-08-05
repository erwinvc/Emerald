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
};