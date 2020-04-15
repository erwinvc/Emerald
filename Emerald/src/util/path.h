#pragma once

class Path {
private:
	String m_dir;
	String m_fileName;
	String m_extention;

public:
	Path() : m_dir(""), m_fileName(""), m_extention("") {}
	Path(const String& dir, const String& fileName, const String& extention) : m_dir(dir), m_fileName(fileName), m_extention(extention) {}
	Path(const String& fullPath) {
		String _fullPath = fullPath;
		for (auto it = _fullPath.find('\\'); it != string::npos; it = _fullPath.find('\\')) _fullPath.replace(it, 1, "/");
		auto periodIndex = _fullPath.find_last_of('.');
		if (periodIndex == string::npos) m_extention = "";
		else m_extention = _fullPath.substr(periodIndex + 1, _fullPath.size());
		auto dashIndex = _fullPath.find_last_of('/') + 1;
		m_fileName = _fullPath.substr(dashIndex, periodIndex + 1 - dashIndex - 1);
		m_dir = _fullPath.substr(0, dashIndex - 1);
	}
	Path(const String_t& fullPath) : Path(String(fullPath)) {}


	String GetFullPath() const {
		return Format("%s/%s.%s", m_dir.c_str(), m_fileName.c_str(), m_extention.c_str());
	}

	String GetFullPathWithoutExtention() const {
		return Format("%s/%s", m_dir.c_str(), m_fileName.c_str());
	}

	String GetDirectory() const {
		return m_dir;
	}

	String GetFileName() const {
		return m_fileName;
	}

	String GetExtention() const {
		return m_extention;
	}
};