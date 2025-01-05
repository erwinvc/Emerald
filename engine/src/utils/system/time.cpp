#include "eepch.h"
#include "time.h"
#pragma comment(lib, "winmm.lib") 
#include "timeapi.h"

namespace emerald {
	void Time::initialize() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		s_ticksToSeconds = 1.0f / freq.QuadPart;
		s_ticksToMilliseconds = 1E+3f / freq.QuadPart;
		s_ticksToMicroseconds = 1E+6f / freq.QuadPart;
		s_ticksToNanoseconds = 1E+9f / freq.QuadPart;
		s_start = getTicks();
	}

	double Time::getTicksToSeconds() { return s_ticksToMilliseconds; }
	double Time::getTicksToMilliseconds() { return s_ticksToMilliseconds; }
	double Time::getTicksToMicroseconds() { return s_ticksToMicroseconds; }
	double Time::getTicksToNanoseconds() { return s_ticksToNanoseconds; }

	int64_t Time::getTicks() {
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return li.QuadPart;
	}

	float Time::getTimeMs() {
		return (float)timeGetTime();

	}
	float Time::getTimeS() {
		return getTimeMs() / 1000;
	}

	bool Time::hasElapsedMs(float startTime, float duration) {
		return (getTimeMs() - startTime) > duration;
	}

	bool Time::hasTickTimeElapsedMs(float startTime, float duration) {
		return (getTickTimeMs<float>() - startTime) > duration;
	}
}