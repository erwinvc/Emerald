#include "eepch.h"
#include "core/application/application.h"
#include "metrics.h"
#include "utils/system/time.h"
#include "utils/threading/threadManager.h"

namespace emerald {
	struct TimerData {
		double m_startTime;
		uint32_t m_frame = 0; //The frame index this measurement was taken
		double m_elapsedTime = 0.0;
	};

	static GLuint s_queryID;
	static std::array<TimerData, (uint32_t)Metric::_COUNT> s_timers;

	void Metrics::initialize() {
		//ASSERT(ThreadManager::isThread(ThreadType::RENDER), "Metrics::initialize must be called on the render thread");
		GL(glGenQueries(1, &s_queryID));
	}

	void Metrics::startTimer(Metric timer) {
		if (timer == Metric::GPU) {
			//ASSERT(ThreadManager::isThread(ThreadType::RENDER), "startTimer(Metric::GPU) must be called on the render thread");

			GL(glBeginQuery(GL_TIME_ELAPSED, s_queryID));
		} else {
			s_timers[(uint32_t)timer].m_startTime = Time::getTickTimeMs<double>();
		}
	}

	void Metrics::endTimer(Metric timer) {
		s_timers[(uint32_t)timer].m_frame = (uint32_t)App->getFrameCount();
		if (timer == Metric::GPU) {
			//ASSERT(ThreadManager::isThread(ThreadType::RENDER), "endTimer(Metric::GPU) must be called on the render thread");

			GL(glEndQuery(GL_TIME_ELAPSED));
			GLuint64 gpuTimeElapsed;
			GL(glGetQueryObjectui64v(s_queryID, GL_QUERY_RESULT, &gpuTimeElapsed));
			s_timers[(uint32_t)timer].m_elapsedTime = gpuTimeElapsed / 1000000.0f;
		} else {
			double endTime = Time::getTickTimeMs<double>();
			s_timers[(uint32_t)timer].m_elapsedTime = endTime - s_timers[(uint32_t)timer].m_startTime;
		}
	}

	float Metrics::getElapsedTime(Metric timer) {
		return (float)s_timers[(uint32_t)timer].m_elapsedTime;
	}

	uint32_t Metrics::getFrame(Metric timer) {
		return s_timers[(uint32_t)timer].m_frame;
	}

	void Metrics::shutdown() {
		//ASSERT(ThreadManager::isThread(ThreadType::RENDER), "Metrics::shutdown must be called on the render thread");
		GL(glDeleteQueries(1, &s_queryID));
	}
}