#include "eepch.h"
#include "time.h"
#pragma comment(lib, "winmm.lib") 
#include "timeapi.h"

namespace emerald {
	void Time::initialize() {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		s_ticksToSeconds = 1.0 / freq.QuadPart;
		s_ticksToMilliseconds = 1E+3 / freq.QuadPart;
		s_ticksToMicroseconds = 1E+6 / freq.QuadPart;
		s_ticksToNanoseconds = 1E+9 / freq.QuadPart;

		s_secondsToTicks = freq.QuadPart / 1.0;
		s_millisecondsToTicks = freq.QuadPart / 1E+3;
		s_microsecondsToTicks = freq.QuadPart / 1E+6;
		s_nanosecondsToTicks = freq.QuadPart / 1E+9f;
		s_start = getTicks();
	}

	double Time::getTicksToSeconds() { return s_ticksToMilliseconds; }
	double Time::getTicksToMilliseconds() { return s_ticksToMilliseconds; }
	double Time::getTicksToMicroseconds() { return s_ticksToMicroseconds; }
	double Time::getTicksToNanoseconds() { return s_ticksToNanoseconds; }

	double Time::getSecondsToTicks() { return s_secondsToTicks; }
	double Time::getMillisecondsToTicks() { return s_millisecondsToTicks; }
	double Time::getMicrosecondsToTicks() { return s_microsecondsToTicks; }
	double Time::getNanosecondsToTicks() { return s_nanosecondsToTicks; }

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