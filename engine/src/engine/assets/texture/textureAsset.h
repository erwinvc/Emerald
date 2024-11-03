#pragma once
#include "../core/asset.h"

namespace emerald {
	class TextureAsset : public Asset {
		bool load() override {  return true; }
		bool reload() override {  return true; }
	};
}