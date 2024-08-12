#pragma once
#ifdef EE_DEBUG
#include "util/log.h"

#define ASSERT(x, ...) \
		if (!(x)) {\
			Log::error("*************************"); \
			Log::error("    ASSERTION FAILED!    "); \
			Log::error("*************************"); \
			Log::error("{}:{}", __FILE__, __LINE__); \
			Log::error("Condition: {}", #x); \
			Log::error(__VA_ARGS__); \
			Log::forceEmptyQueue(); \
			__debugbreak(); \
}
#else
#define ASSERT(x, ...) 
#endif