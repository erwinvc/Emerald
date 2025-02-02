#include "eepch.h"
#include "meshRendererComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	COMPONENT_DEF(MeshRendererComponent);

	nlohmann::json MeshRendererComponent::serialize() {
		nlohmann::json json;
		json["model"] = m_model.getAssetId();
		json["submeshIndex"] = m_submeshIndex;
		return json;
	}

	void MeshRendererComponent::deserialize(const nlohmann::json& json) {
		m_model = AssetRef<Model>(json["model"].get<UUID>());
		auto a = m_model.get();
		m_submeshIndex = json["submeshIndex"];
	}
}