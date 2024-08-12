#include "eepch.h"
#include "utils.h"

namespace emerald::utils {
	uint32_t calculateMipCount(uint32_t width, uint32_t height) {
		return (uint32_t)(std::floor(std::log2(glm::min(width, height))) + 1);
	}
}
