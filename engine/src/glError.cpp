#include "eepch.h"
#include "glError.h"
#include "util/GLUtils.h"

#define HARD_OPENGL_ERROR_CHECK 0

namespace emerald {
	bool glError = false;

	void WINAPI debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;
		Log::warn("GL CALLBACK: type = 0x{0:x}, severity = 0x{1:x}, message = {2}", type, severity, message);
#if HARD_OPENGL_ERROR_CHECK
		Log::forceEmptyQueue();
		glError = true;
#endif
	}

	void GLError::_glGetError() {
		glGetError();
	}

	void GLError::setGLDebugMessageCallback() {
		GL(glDebugMessageCallback(debugMessageCallback, 0));
	}

	bool GLError::glLogCall(const char* function, const char* file, int line) {
#if HARD_OPENGL_ERROR_CHECK
		Log::info("OpenGL: {} in {} at line {}", function, file, line);
#endif

		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			std::string fileName = file;
			size_t position = fileName.find_last_of('\\');
			fileName = fileName.substr(position);
			Log::error("OpenGL: {} caused error {} in {} at line {}", function, GLUtils::glErrorToString(error), fileName.c_str(), line);
			Log::forceEmptyQueue();
			return false;
		}
		return !glError;
	}
}