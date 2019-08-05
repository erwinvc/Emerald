#pragma once

class TimeStep {
private:
    float m_time;
public:

    TimeStep(float initial = 0.0)
        : m_time(initial) {
    }

	operator float() const { return m_time; }

    inline float GetSeconds() const { return m_time / 1000.0f; }
	inline float GetMills() const { return m_time; }

};