#include "eepch.h"
#include "hash.h"

#define XXH_STATIC_LINKING_ONLY
#define XXH_IMPLEMENTATION     
#define XXH_INLINE_ALL
#include "xxhash.h"

namespace emerald {
	uint64_t Hash::hash64(const void* data, std::size_t size) {
		return XXH64(data, size, 0);
	}

	uint32_t Hash::hash32(const void* data, std::size_t size) {
		return XXH32(data, size, 0);
	}
}