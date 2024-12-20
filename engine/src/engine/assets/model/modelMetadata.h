#pragma once
#include "engine/assets/metadata/assetMetadata.h"
#include "model.h"
#include "../loaders/modelLoader.h"

namespace emerald {
	class ModelMetadata : public AssetMetadata {
	public:
		ModelMetadata(const std::filesystem::path& path) : AssetMetadata(AssetType::MODEL, path) {}

		nlohmann::json toJson() const;
		void fromJson(const nlohmann::json& j);

		virtual Ref<AssetLoader> createAssetLoader() override {
			return Ref<ModelLoader>::create(m_path);
		}
	};
}