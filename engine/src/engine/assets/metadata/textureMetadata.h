#pragma once
#include "assetMetadata.h"
#include "graphics/textures/texture.h"
#include "../loaders/textureLoader.h"

namespace emerald {
	class TextureMetadata : public AssetMetadata {
	public:
		TextureMetadata(const std::filesystem::path& path) : AssetMetadata(AssetType::TEXTURE, path) {}

		nlohmann::json toJson() const;
		void fromJson(const nlohmann::json& j);

		virtual Ref<AssetLoader> createAssetLoader() override {
			return Ref<TextureLoader>::create(m_desc, m_path, true);
		}

	private:
		TextureDesc m_desc;
		int m_compression = 0;
	};
}