#include "eepch.h"
#include "timer.h"
#include "time.h"

namespace emerald {
	float Timer::getTime() const {
		return Time::getTimeMs();
	}

	float HighPrecisionTimer::getTime() const {
		return Time::getTickTimeMs();
	}
}