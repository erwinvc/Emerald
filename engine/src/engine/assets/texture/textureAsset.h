#pragma once
#include "engine/assets/core/asset.h"
#include "engine/assets/metadata/assetMetadata.h"

namespace emerald {
	class TextureAsset : public Asset {
	public:
		TextureAsset() = default;

		bool reload() override {  return true; }
	};
}