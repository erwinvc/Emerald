#include "eepch.h"
#include "textureMetadata.h"
#include "core/common/jsonSerializable.h"

namespace emerald {
	nlohmann::json TextureMetadata::toJson() const {
		nlohmann::json j = AssetMetadata::toJson();
		nlohmann::json j2;

		j2["compression"] = 0;
		j["texture"] = j2;

		return j;
	}

	void TextureMetadata::fromJson(const nlohmann::json& j) {
		AssetMetadata::fromJson(j);
		nlohmann::json textureJson = jsonUtils::deserializeRequiredValue<nlohmann::json>(j, "texture");;
		m_compression = jsonUtils::deserializeRequiredValue<uint32_t>(textureJson, "compression");
	}
}