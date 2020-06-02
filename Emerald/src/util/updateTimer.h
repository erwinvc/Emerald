#pragma once

class UpdateTimer {
private:
	Timer m_timer;
	float m_updateTick;
public:
	float m_lastTime;
	int m_elapsedUpdates;
	float m_thisUpdateDelta;
	float m_partialUpdate;

	UpdateTimer(float updatesPerSecond) {
		m_updateTick = 1000.0f / updatesPerSecond;
		m_lastTime = m_timer.Get(Timer::TimeFormat::MILLISECONDS);
	}

	void Update() {
		float time = m_timer.Get(Timer::TimeFormat::MILLISECONDS);
		m_thisUpdateDelta = (float)(time - m_lastTime) / m_updateTick;
		m_lastTime = time;
		m_partialUpdate += m_thisUpdateDelta;
		m_elapsedUpdates = (int)m_partialUpdate;
		m_partialUpdate -= (float)m_elapsedUpdates;
	}
};