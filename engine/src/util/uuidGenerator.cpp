#include "eepch.h"
#include "uuidGenerator.h"
#include "random.h"
#include "time.h"

namespace emerald {
	static std::atomic_uint64_t s_counter;
	static std::mutex s_uuidMutex;
	static uint16_t s_clockSeq = 0;
	static Random s_random;
	static uint32_t s_seed = 0;

	void UUIDGenerator::initialize() {
		static std::once_flag flag;
		std::call_once(flag, []() {
			do {
				s_seed = time::getSystemTime();
				s_random.reset(s_seed);
			} while (!s_seed);
			s_clockSeq = (uint16_t)s_random.getIntInRange(0, 0x3FFF);
		});
	}

	UUID UUIDGenerator::create() {
		std::lock_guard<std::mutex> lock(s_uuidMutex);
		initialize();

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
		uint64_t node = ((uint64_t)(s_random.getInt()) << 32) | s_random.getInt();

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

	UUID UUIDGenerator::createFast() {
		initialize();

		UUID uuid;
		uuid.m_data64[0] = s_counter++;
		uuid.m_data32[2] = s_random.getInt();
		uuid.m_data32[3] = s_seed;
		return uuid;
	}
}