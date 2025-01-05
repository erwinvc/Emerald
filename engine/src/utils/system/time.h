#pragma once

namespace emerald {
	// Provides time information in two different accuracies: based on Ticks (expensive) and based on system time (cheap)
	class Time {
	public:
		static void initialize();

		static int64_t getTicks();

		template<typename T>
		static inline T getTickTimeS() {
			ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class");
			int64_t delta = getTicks() - s_start;
			return (T)(delta * s_ticksToSeconds);
		}

		template<typename T>
		static inline T getTickTimeMs() {
			ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class");
			int64_t delta = getTicks() - s_start;
			return (T)(delta * s_ticksToMilliseconds);
		}

		template<typename T>
		static inline T getTickTimeU() {
			ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class");
			int64_t delta = getTicks() - s_start;
			return (T)(delta * s_ticksToMicroseconds);
		}

		template<typename T>
		static inline T getTickTimeNs() {
			ASSERT(s_start != 0, "Time::initialize() must be called before using the Time class");
			int64_t delta = getTicks() - s_start;
			return (T)(delta * s_ticksToNanoseconds);
		}

		static float getTimeMs();
		static float getTimeS();
		static bool hasElapsedMs(float startTime, float duration);
		static bool hasTickTimeElapsedMs(float startTime, float duration);

	private:
		static double getTicksToSeconds();
		static double getTicksToMilliseconds();
		static double getTicksToMicroseconds();
		static double getTicksToNanoseconds();

		static inline int64_t s_start = 0;
		static inline double s_ticksToSeconds = 0.0f;
		static inline double s_ticksToMilliseconds = 0.0f;
		static inline double s_ticksToMicroseconds = 0.0f;
		static inline double s_ticksToNanoseconds = 0.0f;
	};
}