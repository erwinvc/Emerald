#pragma once

class TimeStep {
private:
    float m_lastUpdate, m_updateTime;
public:
    TimeStep()
        : m_lastUpdate(0.0f), m_updateTime(0.0f) {
    }

    TimeStep(float initial)
        : m_lastUpdate(initial), m_updateTime(initial) {
    }

    inline void Update(float time) {
        m_lastUpdate = time - m_updateTime;
        m_updateTime = time;
    }

    inline float GetMills() const { return m_lastUpdate; }
    inline float GetSeconds() const { return m_lastUpdate * 0.001f; }
};