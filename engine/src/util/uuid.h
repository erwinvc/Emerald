#pragma once

namespace emerald {
	class UUID {
	public:
		UUID();

		UUID(uint32_t uuid) : m_UUID(uuid) {}
		UUID(const UUID& uuid) : m_UUID(uuid.m_UUID) {}

		const std::string& toString() const;

		bool operator==(const UUID& other) const { return m_UUID == other.m_UUID; }
		bool operator!=(const UUID& other) const { return !(*this == other); }
	private:
		uint64_t m_UUID;
	};
}