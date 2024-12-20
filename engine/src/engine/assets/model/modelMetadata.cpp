#include "eepch.h"
#include "modelMetadata.h"
#include "core/application/application.h"

namespace emerald {
	nlohmann::json ModelMetadata::toJson() const {
		nlohmann::json j = AssetMetadata::toJson();
		return j;
	}

	void ModelMetadata::fromJson(const nlohmann::json& j) {
		AssetMetadata::fromJson(j);
	}
}