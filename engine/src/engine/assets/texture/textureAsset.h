#pragma once
#include "engine/assets/core/asset.h"
#include "engine/assets/core/assetMetadata.h"

namespace emerald {
	class TextureAsset : public Asset {
	public:
		TextureAsset() = default;

		bool load() override { return true; }
		bool reload() override {  return true; }
	};
}