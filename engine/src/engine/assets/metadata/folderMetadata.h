#pragma once
#include "assetMetadata.h"

namespace emerald {
	class FolderMetadata : public AssetMetadata {
	public:
		FolderMetadata(const std::filesystem::path& path) : AssetMetadata(AssetType::FOLDER, path) {}

		virtual Ref<AssetLoader> createAssetLoader() {
			return nullptr;
		}
	};
}