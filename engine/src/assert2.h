#pragma once

#ifdef EE_DEBUG
#define ASSERT(x, ...) \
		if (!(x)) {\
			Log::info("*************************"); \
			Log::info("    ASSERTION FAILED!    "); \
			Log::info("*************************"); \
			Log::info("{}{}", __FILE__, __LINE__); \
			Log::info("Condition: {}", #x); \
			Log::error(__VA_ARGS__); \
			__debugbreak(); \
}
#else
#define ASSERT(x, ...) 
#endif