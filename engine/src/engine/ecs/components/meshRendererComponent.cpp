#include "eepch.h"
#include "meshRendererComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	RTTI_CLASS_DEF(MeshRendererComponent);

	nlohmann::json MeshRendererComponent::serialize() {
		nlohmann::json json;

		return json;
	}

	MeshRendererComponent* MeshRendererComponent::deserialize(const nlohmann::json& json, Entity entity) {
		MeshRendererComponent* comp = ECSManager::ECS().addComponent<MeshRendererComponent>(entity);

		return comp;
	}
}