#include "eepch.h"
#include "uuid.h"
#include <random>

namespace emerald {
	static std::atomic_ulong m_counter;
	static std::random_device s_randomDevice;
	static std::mt19937_64 s_gen(s_randomDevice());
	static std::uniform_int_distribution<uint64_t> s_uniformDistribution;

	//UUID::UUID() : m_UUID(s_uniformDistribution(s_gen)) {}
	//
	//UUID createFast() {
	//	UUID guid;
	//	guid.m_data64[0] = m_counter++;
	//	guid.m_data32[2] = static_cast<uint32_t>(m_rng.GetInt());
	//	guid.m_data32[3] = m_seed;
	//	return guid;
	//	return UUID(s_uniformDistribution(s_gen));
	//}

	//const std::string UUID::toString() const {
	//	std::stringstream ss;
	//	ss << std::hex << std::setw(16) << std::setfill('0') << m_UUID << std::setw(16) << std::setfill('0') << m_UUID;
	//	return ss.str();
	//}
}