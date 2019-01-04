#pragma once

#include <chrono>

typedef std::chrono::high_resolution_clock HighResolutionClock;
typedef std::chrono::duration<float, std::milli> milliseconds_type;

class Timer
{
private:
    std::chrono::time_point<HighResolutionClock> m_start;
public:

    Timer() : m_start(HighResolutionClock::now()) { }

    float GetNano() {
        return std::chrono::duration_cast<milliseconds_type>(HighResolutionClock::now() - m_start).count() / 1000.0f;
    }

    float Get() {
        return std::chrono::duration_cast<milliseconds_type>(HighResolutionClock::now() - m_start).count();
    }
};

