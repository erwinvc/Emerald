#pragma once
#include <string>

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

		std::string toString() const;
		bool fromString(const std::string& str);

		operator bool() const { return isValid(); }
		bool operator==(const UUID& other) const {	return m_data64[0] == other.m_data64[0] && m_data64[1] == other.m_data64[1]; }
		bool operator!=(const UUID& other) const { return !(*this == other); }
		operator std::string() const { return toString(); }

	private:
		friend struct std::hash<UUID>; // Grant access to std::hash
		friend class UUIDGenerator;

		union {
			uint64_t m_data64[2];
			uint32_t m_data32[4];
			uint16_t m_data16[8];
			uint8_t m_data8[16];
		};
	};
}

namespace std {
	template <>
	struct hash<emerald::UUID> {
		std::size_t operator()(const emerald::UUID& uuid) const noexcept {
			std::size_t h1 = std::hash<uint64_t>{}(uuid.m_data64[0]);
			std::size_t h2 = std::hash<uint64_t>{}(uuid.m_data64[1]);

			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}