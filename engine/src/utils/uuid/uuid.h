#pragma once
#include <string>
#include <array>
#include "utils/core/hash.h"

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

		explicit operator bool() const { return isValid(); }
		bool operator==(const UUID& other) const {	return m_data64[0] == other.m_data64[0] && m_data64[1] == other.m_data64[1]; }
		bool operator!=(const UUID& other) const { return !(*this == other); }
		operator std::string() const { return toString(); }

	private:
		friend struct std::hash<UUID>;
		friend class UUIDGenerator;

		union {
			std::array<uint64_t, 2> m_data64;
			std::array<uint32_t, 4> m_data32;
			std::array<uint16_t, 8> m_data16;
			std::array<uint8_t, 16> m_data8;
		};
	};

	inline void to_json(nlohmann::json& j, const UUID& uuid) {
		j = uuid.toString();
	}
	inline void from_json(const nlohmann::json& j, UUID& uuid) {
		uuid.fromString(j.get<std::string>());
	}
}

namespace std {
	template <>
	struct formatter<emerald::UUID> : formatter<std::string> {
		constexpr auto parse(format_parse_context& ctx) {
			return ctx.begin();
		}

		template <typename FormatContext>
		auto format(const emerald::UUID& uuid, FormatContext& ctx) {
			return formatter<std::string>::format(uuid.toString(), ctx);
		}
	};

	template <>
	struct hash<emerald::UUID> {
		std::size_t operator()(const emerald::UUID& uuid) const noexcept {
			auto hash64 = emerald::Hash::hash64(uuid.m_data8.data(), 16);
			return static_cast<std::size_t>(hash64);
		}
	};
}