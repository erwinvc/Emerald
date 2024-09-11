#pragma once

namespace emerald::time {
	uint32_t getSystemTime();
	bool hasElapsed(uint32_t startTime, uint32_t duration);
}