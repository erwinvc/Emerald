#pragma once

typedef std::chrono::high_resolution_clock HighResolutionClock;
typedef std::chrono::duration<float, std::nano> ChronoNano;
typedef std::chrono::duration<float, std::micro> ChronoMicro;
typedef std::chrono::duration<float, std::milli> ChronoMilli;
typedef std::chrono::duration<float, std::deca> ChronoSeconds;

class Timer {
private:
	chrono::time_point<HighResolutionClock> m_start;
public:
	enum TimeFormat { NANOSECONDS, MICROSECONDS, MILLISECONDS, SECONDS };

	Timer() : m_start(HighResolutionClock::now()) {}

	float Get(TimeFormat fmt = MILLISECONDS) {
		switch (fmt) {
		case NANOSECONDS: return std::chrono::duration_cast<ChronoNano>(HighResolutionClock::now() - m_start).count();
		case MICROSECONDS: return std::chrono::duration_cast<ChronoMicro>(HighResolutionClock::now() - m_start).count();
		case MILLISECONDS: return std::chrono::duration_cast<ChronoMilli>(HighResolutionClock::now() - m_start).count();
		case SECONDS: return std::chrono::duration_cast<ChronoSeconds>(HighResolutionClock::now() - m_start).count();
		}
		return -1;
	}

	void Print() {
		LOG("%.2fms", Get());
	}
};

