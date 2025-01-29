#pragma once
#include "utils/text/fixedString.h"

namespace emerald {
	class Hash {
	public:
		static uint64_t hash64(const void* data, std::size_t size);
		static uint32_t hash32(const void* data, std::size_t size);

		template <typename T>
		static uint32_t hash32(const T& str);

		template <>
		static inline uint32_t hash32<std::string>(const std::string& str) {
			return Hash::hash32(str.data(), str.length());
		}

		template <std::size_t N>
		static inline uint32_t hash32(const FixedString<N>& str) {
			return Hash::hash32(str.data(), str.length());
		}

		template <typename T>
		static uint64_t hash64(const T& str);

		template <>
		static inline uint64_t hash64<std::string>(const std::string& str) {
			return Hash::hash64(str.data(), str.length());
		}

		template <std::size_t N>
		static inline uint64_t hash64(const FixedString<N>& str) {
			return Hash::hash64(str.data(), str.length());
		}
	};
}