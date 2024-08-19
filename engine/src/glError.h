#pragma once

namespace emerald {
	namespace GLError {
		void GLAPIENTRY debugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
		void setGLDebugMessageCallback();
		bool glLogCall(const char* function, const char* file, int line);
	}

#ifdef EE_DEBUG
#define GL(x) glGetError();\
		x; \
		if (!GLError::glLogCall(#x, __FILE__, __LINE__)) __debugbreak();
#else
#define GL(x) x
#endif
}