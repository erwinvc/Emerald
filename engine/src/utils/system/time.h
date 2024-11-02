#pragma once

namespace emerald {
	// Provides time information in two different accuracies: based on Ticks (expensive) and based on system time (cheap)
	class Time {
	public:
		static void initialize();

		static int64_t getTicks();
		static float getTickTimeS();
		static float getTickTimeMs();
		static float getTickTimeU();
		static float getTickTimeNs();
		static float getTimeMs();
		static float getTimeS();
		static bool hasElapsedMs(float startTime, float duration);
		static bool hasTickTimeElapsedMs(float startTime, float duration);

	private:
		static float getTicksToSeconds();
		static float getTicksToMilliseconds();
		static float getTicksToMicroseconds();
		static float getTicksToNanoseconds();
	};
}