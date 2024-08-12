#pragma once
#include <cstdint>
#include <string>

namespace emerald {
	struct RenderCapabilities {
		std::string vendor;
		std::string version;

		uint32_t maxTextureSize;
		uint32_t maxTextureUnits;
		uint32_t maxColorAttachments;
		uint32_t maxDrawBuffers;
		uint32_t maxSamples;
		uint32_t maxAnisotropy;
	};
}