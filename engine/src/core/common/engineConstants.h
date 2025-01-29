#pragma once
#include "stdint.h"

namespace emerald {
	class EngineConstants {
	public:
		static constexpr uint32_t VERSION_SCENESERIALIZATION = 1;
		static constexpr uint32_t VERSION_ASSETMETADATA = 1;
		static constexpr uint32_t VERSION_PROJECT = 1;
	};
}