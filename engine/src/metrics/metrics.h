#pragma once

namespace emerald {
	enum class Metric {
		LOGIC,
		RENDER,
		LOGICWAIT,
		RENDERWAIT,
		GPU,
		_COUNT
	};

	class Metrics {
	public:
		static void initialize();
		static void startTimer(Metric timer);
		static void endTimer(Metric timer);
		static float getElapsedTime(Metric timer);
		static uint32_t getFrame(Metric timer);
		static void shutdown();
	};
}