#pragma once

class TimeStep {
private:
	float m_time;
	float m_totalTime;
	uint64 m_frameCount;
public:

	TimeStep(float initial = 0.0, float totalTime = 0.0f, uint64 frameCount = 0)
		: m_time(initial), m_totalTime(totalTime), m_frameCount(frameCount) {
	}

	operator float() const { return m_time; }

	inline float GetSeconds() const { return m_time / 1000.0f; }
	inline float GetMills() const { return m_time; }

	inline float TotalTime() const { return m_totalTime; }
	inline uint64 FrameCount() const { return m_frameCount; }
};