#pragma once

class Path {
private:
	String m_dir;
	String m_fileName;
	String m_extention;

public:
	Path() : m_dir(""), m_fileName(""), m_extention("") {}
	Path(const String& dir, const String& fileName, const String& extention) : m_dir(dir), m_fileName(fileName), m_extention(extention) {}
	Path(const String& fullPath);
	Path(const String_t& fullPath) : Path(String(fullPath)) {}

	inline String GetFullPath() const { return Format("%s/%s.%s", m_dir.c_str(), m_fileName.c_str(), m_extention.c_str()); }
	inline String GetFullPathWithoutExtention() const { return Format("%s/%s", m_dir.c_str(), m_fileName.c_str()); }
	inline String GetDirectory() const { return m_dir; }
	inline String GetFileName() const { return m_fileName; }
	inline String GetExtention() const { return m_extention; }
	inline bool Exists() const { return FileSystem::DoesFileExist(GetFullPath()); }
};