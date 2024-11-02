#pragma once
#ifdef EE_DEBUG
#include "utils/core/log.h"

template <typename T, typename = void>
struct has_getClassType : std::false_type {};

template <typename T>
struct has_getClassType<T, std::void_t<decltype(T::getStaticClassType())>> : std::true_type {};


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

#define ASSERT_RTTI(T) \
		static_assert(has_getClassType<T>::value, "RTTI assertion failed: " #T " has not declared RTTI"); \
        ASSERT(T::template checkRTTI<T>(), "RTTI assertion failed: " #T " has not defined RTTI"); 

#else
#define ASSERT(x, ...) 
#define ASSERT_RTTI(T) 
#endif