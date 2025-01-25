#include "eepch.h"
#include "metadataComponent.h"
#include "engine/scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	RTTI_CLASS_DEF(MetadataComponent);

	nlohmann::json MetadataComponent::serialize() {
		nlohmann::json json;
		
		json["name"] = m_name;
		json["flags"] = m_flags;

		return json;
	}

	MetadataComponent* MetadataComponent::deserialize(const nlohmann::json& json, Entity entity) {
		MetadataComponent* comp = ECSManager::ECS().addComponent<MetadataComponent>(entity, json["name"]);
		comp->m_flags = json["flags"];
		return comp;
	}
}