#pragma once

namespace emerald {
	class UUID {
	public:
		UUID() { clear(); }
		UUID(std::string str) { fromString(str); }

		bool isValid() const { return !(m_data64[0] == 0 && m_data64[1] == 0); }

		void clear() {
			m_data64[0] = 0;
			m_data64[1] = 0;
		}

		inline bool operator==(const UUID& other) { return m_data64[0] == other.m_data64[0] && m_data64[1] == other.m_data64[1]; }
		inline bool operator!=(const UUID& other) { return !operator==(other); }

		// Convert UUID to string
		std::string toString() const {
			std::ostringstream oss;
			oss << std::hex << std::setfill('0');
			for (int i = 0; i < 16; ++i) {
				oss << std::setw(2) << static_cast<int>(m_data8[i]);
				if (i == 3 || i == 5 || i == 7 || i == 9) {
					oss << '-';  // UUID format with dashes
				}
			}
			return oss.str();
		}

		// Conversion operator: implicitly convert UUID to std::string
		operator std::string() const {
			return toString();
		}

		// Parse string to UUID
		bool fromString(const std::string& str) {
			clear();
			if (str.size() != 36) return false;  // Must be 36 characters

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

	private:
		friend class UUIDGenerator;
		union {
			uint64_t m_data64[2];
			uint32_t m_data32[4];
			uint16_t m_data16[8];
			uint8_t m_data8[16];
		};
	};
}