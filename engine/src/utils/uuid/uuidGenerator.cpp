#include "eepch.h"
#include "uuidGenerator.h"
#include "utils/math/random.h"
#include "utils/system/time.h"
#include <mutex>

namespace emerald {
	static std::atomic_uint64_t s_counter;
	static std::mutex s_uuidMutex;
	static uint16_t s_clockSeq = 0;

	UUID UUIDGenerator::createVersion1() {
		std::lock_guard<std::mutex> lock(s_uuidMutex);

		UUID uuid;

		using namespace std::chrono;
		auto now = system_clock::now();
		auto timeSinceEpoch = now.time_since_epoch();
		auto uuidTime = duration_cast<nanoseconds>(timeSinceEpoch).count() / 100;

		uint32_t timeLow = (uint32_t)(uuidTime & 0xFFFFFFFF);               // Low 32 bits
		uint16_t timeMid = (uint16_t)((uuidTime >> 32) & 0xFFFF);           // Mid 16 bits
		uint16_t timeHiAndVersion = (uint16_t)((uuidTime >> 48) & 0x0FFF);  // High 12 bits

		// Set the version to 1 (time-based UUID)
		timeHiAndVersion |= (1 << 12);

		// Clock sequence
		uint8_t clockSeqHi = (uint8_t)((s_clockSeq >> 8) & 0x3F);  // Top 6 bits
		clockSeqHi |= 0x80;  // Set the two most significant bits to 10, as per the RFC
		uint8_t clockSeqLow = (uint8_t)(s_clockSeq & 0xFF);  // Low 8 bits

		// Node address (random 48-bit number)
		uint64_t node = ((uint64_t)(StaticRandom::getInt()) << 32) | StaticRandom::getInt();

		// Assign the components to the UUID
		uuid.m_data32[0] = timeLow;
		uuid.m_data16[2] = timeMid;
		uuid.m_data16[3] = timeHiAndVersion;
		uuid.m_data8[8] = clockSeqHi;
		uuid.m_data8[9] = clockSeqLow;

		// Node (MAC address or random)
		uuid.m_data8[10] = (uint8_t)((node >> 40) & 0xFF);
		uuid.m_data8[11] = (uint8_t)((node >> 32) & 0xFF);
		uuid.m_data8[12] = (uint8_t)((node >> 24) & 0xFF);
		uuid.m_data8[13] = (uint8_t)((node >> 16) & 0xFF);
		uuid.m_data8[14] = (uint8_t)((node >> 8) & 0xFF);
		uuid.m_data8[15] = (uint8_t)(node & 0xFF);

		s_clockSeq = (s_clockSeq + 1) & 0x3FFF;

		return uuid;
	}

	UUID UUIDGenerator::create() {
		//std::lock_guard<std::mutex> lock(s_uuidMutex);

		UUID uuid;

		// Generate 128 bits of random data
		// Assuming s_random.getInt() returns a random uint32_t
		uint32_t r0 = StaticRandom::getInt();
		uint32_t r1 = StaticRandom::getInt();
		uint32_t r2 = StaticRandom::getInt();
		uint32_t r3 = StaticRandom::getInt();

		uuid.m_data32[0] = r0;
		uuid.m_data32[1] = r1;
		uuid.m_data32[2] = r2;
		uuid.m_data32[3] = r3;

		// Set the version to 4 (random UUID)
		// Version is stored in the 4 most significant bits of the timeHiAndVersion field (m_data16[3]).
		// Clear these bits (0x0FFF) and then set them to 0x4000.
		uuid.m_data16[3] = (uuid.m_data16[3] & 0x0FFF) | 0x4000;

		// Set the two most significant bits of the clockSeqHi field to 10
		// This ensures the UUID variant is correctly set to "RFC 4122".
		// The clockSeqHi is at m_data8[8]. Clearing the top two bits and then setting them to 10 (binary)
		// is done by: & 0x3F to clear top two bits, then | 0x80 to set them to 10.
		uuid.m_data8[8] = (uuid.m_data8[8] & 0x3F) | 0x80;

		return uuid;
	}

	UUID UUIDGenerator::createFast() {
		uint64_t t = Time::getTickTimeNs<uint64_t>();
		uint64_t r = (static_cast<uint64_t>(StaticRandom::getInt()) << 32) | static_cast<uint64_t>(StaticRandom::getInt());

		UUID uuid;
		uuid.m_data64[0] = t;
		uuid.m_data64[1] = r;

		// 3) Set the "version" bits (bits 12..15 of timeHiAndVersion) to 4
		//    which indicates a (pseudo-)random or "fast" UUID.
		//    m_data16[3] is the high 16 bits of the second 32-bit block
		//    or equivalently bits [48..63] in the 128-bit data.
		uuid.m_data16[3] = static_cast<uint16_t>((uuid.m_data16[3] & 0x0FFF) | (4 << 12));

		// 4) Set the "variant" bits (bits 6..7 of clockSeqHi) to binary 10
		//    which indicates RFC 4122 style. That lives in m_data8[8].
		uuid.m_data8[8] = static_cast<uint8_t>((uuid.m_data8[8] & 0x3F) | 0x80);
		return uuid;
	}
}