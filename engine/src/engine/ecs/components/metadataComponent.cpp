#include "eepch.h"
#include "metadataComponent.h"
#include "engine/scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	COMPONENT_DEF(MetadataComponent);

	nlohmann::json MetadataComponent::serialize() {
		nlohmann::json json;
		
		json["name"] = m_name;
		json["flags"] = m_flags;

		return json;
	}

	void MetadataComponent::deserialize(const nlohmann::json& json) {
		std::string name = json["name"];
		m_name = FixedString<NAME_MAX_LENGTH>(name.c_str());
		m_flags = json["flags"];
	}
}