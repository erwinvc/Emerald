#include "eepch.h"
#include "metadataComponent.h"
#include "engine/scene/sceneManager.h"

namespace emerald {
	RTTI_CLASS_DEF(MetadataComponent);

	nlohmann::json MetadataComponent::serialize() {
		nlohmann::json json;
		
		json["name"] = m_name;
		json["flags"] = m_flags;

		return json;
	}

	MetadataComponent* MetadataComponent::deserialize(const nlohmann::json& json, Entity entity) {
		EntityComponentSystem& ecs = SceneManager::getActiveScene()->getECS();
		MetadataComponent* comp = ecs.addComponent<MetadataComponent>(entity, json["name"]);
		comp->m_flags = json["flags"];
		return comp;
	}
}