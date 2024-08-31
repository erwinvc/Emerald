#pragma once
#include <cstdint>
#include <string>

namespace emerald::utils {
	uint32_t calculateMipCount(uint32_t width, uint32_t height);
	std::wstring stringToWstring(const std::string& str);

	template<typename T>
	void eraseFromVector(std::vector<T>& vec, const T& value) {
		vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
	}
}