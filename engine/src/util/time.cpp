#include "eepch.h"
#include "time.h"
#pragma comment(lib, "winmm.lib") 
#include "timeapi.h"

namespace emerald::time {
	uint32_t getSystemTime() {
		return timeGetTime();
	}
	bool hasElapsed(uint32_t startTime, uint32_t duration) {
		return (getSystemTime() - startTime) > duration;
	}
}