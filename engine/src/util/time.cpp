#include "eepch.h"
#include "time.h"
#pragma comment(lib, "winmm.lib") 
#include "timeapi.h"

namespace emerald {
	static float s_ticksToSeconds = 0.0f;
	static float s_ticksToMilliseconds = 0.0f;
	static float s_ticksToMicroseconds = 0.0f;
	static float s_ticksToNanoseconds = 0.0f;
	static int64_t s_start = 0;

	void Time::initialize() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		s_ticksToSeconds = 1.0f / freq.QuadPart;
		s_ticksToMilliseconds = 1E+3f / freq.QuadPart;
		s_ticksToMicroseconds = 1E+6f / freq.QuadPart;
		s_ticksToNanoseconds = 1E+9f / freq.QuadPart;
		s_start = getTicks();
	}

	float Time::getTicksToSeconds() { return s_ticksToMilliseconds; }
	float Time::getTicksToMilliseconds() { return s_ticksToMilliseconds; }
	float Time::getTicksToMicroseconds() { return s_ticksToMicroseconds; }
	float Time::getTicksToNanoseconds() { return s_ticksToNanoseconds; }

	int64_t Time::getTicks() {
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	float Time::getTickTimeS() {
		ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class")
		int64_t delta = getTicks() - s_start;
		return delta * s_ticksToSeconds;
	}

	float Time::getTickTimeMs() {
		ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class")
		int64_t delta = getTicks() - s_start;
		return delta * s_ticksToMilliseconds;
	}

	float Time::getTickTimeU() {
		ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class")
		int64_t delta = getTicks() - s_start;
		return delta * s_ticksToMicroseconds;
	}

	float Time::getTickTimeNs() {
		ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class")
		int64_t delta = getTicks() - s_start;
		return delta * s_ticksToNanoseconds;
	}

	float Time::getTimeMs() {
		return timeGetTime();

	}
	float Time::getTimeS() {
		return getTimeMs() / 1000;
	}

	bool Time::hasElapsedMs(float startTime, float duration) {
		return (getTimeMs() - startTime) > duration;
	}

	bool Time::hasTickTimeElapsedMs(float startTime, float duration) {
		return (getTickTimeMs() - startTime) > duration;
	}
}