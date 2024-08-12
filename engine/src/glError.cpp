#include "eepch.h"
#include "glError.h"
#include "util/GLUtils.h"

namespace emerald {
	bool glLogCall(const char* function, const char* file, int line) {
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::string fileName = file;
			size_t position = fileName.find_last_of('\\');
			fileName = fileName.substr(position);
			Log::error("OpenGL: {} caused error {} in {} at line {}", function, GLUtils::glErrorToString(error), fileName.c_str(), line);
			Log::forceEmptyQueue();
			return false;
		}
		return true;
	}
}