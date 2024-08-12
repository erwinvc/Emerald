#pragma once

namespace emerald {
	bool glLogCall(const char* function, const char* file, int line);

#ifdef EE_DEBUG
#define GL(x) glGetError();\
		x; \
		if (!glLogCall(#x, __FILE__, __LINE__)) __debugbreak();
#else
#define GL(x) x
#endif
}