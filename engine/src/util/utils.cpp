#include "eepch.h"
#include "utils.h"

namespace emerald::utils {
	uint32_t calculateMipCount(uint32_t width, uint32_t height) {
		return (uint32_t)(std::floor(std::log2(glm::min(width, height))) + 1);
	}
	std::wstring stringToWstring(const std::string& str) {
		int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
		std::wstring wstrTo(size_needed, 0);
		MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
		return wstrTo;
	}
}
