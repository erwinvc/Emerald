#include "eepch.h"
#include "uuid.h"
#include <random>

namespace emerald {
	std::string UUID::toString() const {
		static const char* hex_chars = "0123456789abcdef";
		std::string result(36, '\0'); // UUIDs are always 36 characters (32 hex + 4 dashes)

		int index = 0;
		for (int i = 0; i < 16; ++i) {
			result[index++] = hex_chars[(m_data8[i] >> 4) & 0xF];
			result[index++] = hex_chars[m_data8[i] & 0xF];
			if (i == 3 || i == 5 || i == 7 || i == 9) {
				result[index++] = '-';
			}
		}
		return result;
	}

	bool UUID::fromString(const std::string& str) {
		clear();
		if (str.size() != 36) return false;

		static const int dashPositions[] = { 8, 13, 18, 23 };
		int dashIndex = 0;

		for (int i = 0, byteIndex = 0; i < 36; ++i) {
			char c = str[i];
			if (dashIndex < 4 && i == dashPositions[dashIndex]) {
				if (c != '-') return false;
				++dashIndex;
				continue;
			}

			if (!std::isxdigit(c)) return false;

			uint8_t nibble = (c >= '0' && c <= '9') ? c - '0'
				: (c >= 'a' && c <= 'f') ? c - 'a' + 10
				: c - 'A' + 10;

			if (i % 2 == 0) {
				m_data8[byteIndex] = nibble << 4;
			} else {
				m_data8[byteIndex++] |= nibble; 
			}
		}

		return true;
	}
}