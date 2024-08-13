#pragma once
#include <chrono>
#include "log.h"

typedef std::chrono::high_resolution_clock HighResolutionClock;
typedef std::chrono::duration<float, std::nano> ChronoNano;
typedef std::chrono::duration<float, std::micro> ChronoMicro;
typedef std::chrono::duration<float, std::milli> ChronoMilli;
typedef std::chrono::duration<float, std::deca> ChronoSeconds;

namespace emerald {
	class Timer {
	private:
		std::chrono::time_point<HighResolutionClock> m_start;
	public:
		enum TimeFormat { NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS };

		Timer() : m_start(HighResolutionClock::now()) {}

		float get(TimeFormat fmt = MILLISECONDS) {
			switch (fmt) {
				case NANOSECONDS: return std::chrono::duration_cast<ChronoNano>(HighResolutionClock::now() - m_start).count();
				case MICROSECONDS: return std::chrono::duration_cast<ChronoMicro>(HighResolutionClock::now() - m_start).count();
				case MILLISECONDS: return std::chrono::duration_cast<ChronoMilli>(HighResolutionClock::now() - m_start).count();
				case SECONDS: return std::chrono::duration_cast<ChronoSeconds>(HighResolutionClock::now() - m_start).count();
			}
			return -1;
		}

		void print() {
			Log::info("{:.2f}ms", get());
		}

		void print(std::string name) {
			Log::info("{} took {:.2f}ms", name, get());
		}
	};
}