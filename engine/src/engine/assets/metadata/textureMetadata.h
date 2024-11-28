#pragma once
#include "assetMetadata.h"

namespace emerald {
	class TextureMetadata : public AssetMetadata {
	public:
		nlohmann::json toJson() const;
		void fromJson(const nlohmann::json& j);

	private:
		int m_compression = 0;
	};
}