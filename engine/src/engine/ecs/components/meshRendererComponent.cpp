#include "eepch.h"
#include "meshRendererComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"

namespace emerald {
	RTTI_CLASS_DEF(MeshRendererComponent);

	nlohmann::json MeshRendererComponent::serialize() {
		nlohmann::json json;

		return json;
	}

	MeshRendererComponent* MeshRendererComponent::deserialize(const nlohmann::json& json, Entity entity) {
		EntityComponentSystem& ecs = SceneManager::getActiveScene()->getECS();
		MeshRendererComponent* comp = ecs.addComponent<MeshRendererComponent>(entity);

		return comp;
	}
}