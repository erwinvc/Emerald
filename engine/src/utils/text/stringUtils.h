#pragma once
#include <string>
#include "fixedString.h"

#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION     
#define XXH_INLINE_ALL
#include "xxhash.h"

namespace emerald {
	class StringUtils {
	public:
		static inline std::string toLower(const std::string& str) {
			std::string result = str;
			std::transform(result.begin(), result.end(), result.begin(), ::tolower);
			return result;
		}

		template <typename T>
		static uint32_t hash32(const T& str);

		template <>
		static inline uint32_t hash32<std::string>(const std::string& str) {
			return XXH32(str.data(), str.length(), 0);
		}

		template <std::size_t N>
		static inline uint32_t hash32(const FixedString<N>& str) {
			return XXH32(str.data(), str.length(), 0);
		}

		template <typename T>
		static uint64_t hash64(const T& str);

		template <>
		static inline uint64_t hash64<std::string>(const std::string& str) {
			return XXH64(str.data(), str.length(), 0);
		}

		template <std::size_t N>
		static inline uint64_t hash64(const FixedString<N>& str) {
			return XXH64(str.data(), str.length(), 0);
		}
	};
}