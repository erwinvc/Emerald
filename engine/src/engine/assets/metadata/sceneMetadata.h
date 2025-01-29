#pragma once
#include "assetMetadata.h"

namespace emerald {
	class SceneMetadata : public AssetMetadata {
	public:
		SceneMetadata(const std::filesystem::path& path) : AssetMetadata(AssetType::SCENE, path) {}

		virtual Ref<AssetLoader> createAssetLoader() {
			return nullptr;
		}
	};
}