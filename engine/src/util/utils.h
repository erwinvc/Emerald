#pragma once
#include <cstdint>
#include <string>

namespace emerald::utils {
	uint32_t calculateMipCount(uint32_t width, uint32_t height);
	std::wstring stringToWstring(const std::string& str);

	template<typename T>
	bool eraseFromVector(std::vector<T>& vec, const T& item) {
		auto it = std::find(vec.begin(), vec.end(), item);
		if (it != vec.end()) {
			vec.erase(it);
			return true; 
		}
		return false;
	}

	template<typename T>
	uint32_t getIndexInVector(std::vector<T> v, T K) {
		auto it = find(v.begin(), v.end(), K);

		if (it != v.end()) {
			return it - v.begin();
		} else {
			return -1;
		}
	}
}