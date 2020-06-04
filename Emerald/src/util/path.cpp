#include "stdafx.h"

Path::Path(const String& fullPath) {
	String _fullPath = fullPath;
	for (auto it = _fullPath.find('\\'); it != string::npos; it = _fullPath.find('\\')) _fullPath.replace(it, 1, "/");
	auto periodIndex = _fullPath.find_last_of('.');
	if (periodIndex == string::npos) m_extention = "";
	else m_extention = _fullPath.substr(periodIndex + 1, _fullPath.size());
	auto dashIndex = _fullPath.find_last_of('/') + 1;
	m_fileName = _fullPath.substr(dashIndex, periodIndex + 1 - dashIndex - 1);
	m_dir = _fullPath.substr(0, dashIndex - 1);
}