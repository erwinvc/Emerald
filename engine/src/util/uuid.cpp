#include "eepch.h"
#include "uuid.h"
#include <random>

namespace emerald {
	std::string UUID::toString() const {
		std::ostringstream oss;
		oss << std::hex << std::setfill('0');
		for (int i = 0; i < 16; ++i) {
			oss << std::setw(2) << static_cast<int>(m_data8[i]);
			if (i == 3 || i == 5 || i == 7 || i == 9) {
				oss << '-';
			}
		}
		return oss.str();
	}

	bool UUID::fromString(const std::string& str) {
		clear();
		if (str.size() != 36) return false;

		std::string cleanStr;
		for (char c : str) {
			if (c != '-') cleanStr += c;
		}

		if (cleanStr.size() != 32) return false;

		for (int i = 0; i < 16; ++i) {
			std::string byteStr = cleanStr.substr(i * 2, 2);
			m_data8[i] = static_cast<uint8_t>(std::stoul(byteStr, nullptr, 16));
		}
		return true;
	}
}