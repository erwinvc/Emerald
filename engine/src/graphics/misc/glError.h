#pragma once

namespace emerald {
	namespace GLError {
		void setGLDebugMessageCallback();
		bool glLogCall(const char* function, const char* file, int line);
		void _glGetError();
	}

#ifdef EE_DEBUG
#define GL(x) GLError::_glGetError();\
		x; \
		if (!GLError::glLogCall(#x, __FILE__, __LINE__)) __debugbreak();
#else
#define GL(x) x
#endif
}