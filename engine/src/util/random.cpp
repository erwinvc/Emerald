#include "eepch.h"
#include "random.h"
#include "time.h"
#include <random>

namespace emerald {
	Random& StaticRandom::getThreadLocalRandom() {
		static thread_local Random threadLocalRandomInstance = initializeRandom();
		return threadLocalRandomInstance;
	}

	Random StaticRandom::initializeRandom() {
		std::random_device rd;
		auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());  // Unique thread ID
		auto time_stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();  // Current time
		uint32_t seed = rd() ^ (static_cast<uint32_t>(thread_id) + static_cast<uint32_t>(time_stamp));
		return Random(seed);
	}
}