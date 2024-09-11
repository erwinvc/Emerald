#pragma once

namespace emerald {
	class Debug {
	public:
		static void initialize();
		static void shutdown();
		static std::string captureStackTrace(uint32_t framesToSkip, uint32_t frameCount = 64, bool writeAddress = false);
	};
}