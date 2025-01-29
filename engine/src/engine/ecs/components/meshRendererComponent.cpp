#include "eepch.h"
#include "meshRendererComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	COMPONENT_DEF(MeshRendererComponent);

	nlohmann::json MeshRendererComponent::serialize() {
		nlohmann::json json;

		return json;
	}

	void MeshRendererComponent::deserialize(const nlohmann::json& json) {
	}
}