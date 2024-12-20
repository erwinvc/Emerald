#pragma once
#include "assetMetadata.h"

namespace emerald {
	class DefaultMetadata : public AssetMetadata {
	public:
		DefaultMetadata(const std::filesystem::path& path) : AssetMetadata(AssetType::DEFAULT, path) {}

		virtual Ref<AssetLoader> createAssetLoader() {
			return nullptr;
		}
	};
}