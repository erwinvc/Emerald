#pragma once
#include "assetMetadata.h"

namespace emerald {
	class TextureMetadata : public AssetMetadata {
	public:
		nlohmann::json toJson() const;
		static void fromJson(const nlohmann::json& j, TextureMetadata& metaData);

	private:
		int m_compression = 0;
	};
}