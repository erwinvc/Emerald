#pragma once

static String ReadFile(String path) {
	ifstream stream(path);
	string str((istreambuf_iterator<char>(stream)), istreambuf_iterator<char>());
	stream.close();
	return str;
}