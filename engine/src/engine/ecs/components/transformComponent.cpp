#include "eepch.h"
#include "transformComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"

namespace emerald {
	RTTI_CLASS_DEF(TransformComponent);

	void TransformComponent::updateLocalTransform() {
		m_localTransform = glm::mat4(1.0f);

		m_localTransform = glm::translate(m_localTransform, m_position);

		m_localTransform = glm::rotate(m_localTransform, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		m_localTransform = glm::rotate(m_localTransform, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		m_localTransform = glm::rotate(m_localTransform, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

		m_localTransform = glm::scale(m_localTransform, m_scale);

		m_needsUpdate = false;
	}

	void TransformComponent::updateGlobalTransform(const glm::mat4& parentTransform) {
		if (m_needsUpdate) updateLocalTransform();
		m_globalTransform = parentTransform * m_localTransform;
	}

	nlohmann::json TransformComponent::serialize() {
		nlohmann::json json;
		json["position"] = { m_position.x, m_position.y, m_position.z };
		json["rotation"] = { m_rotation.x, m_rotation.y, m_rotation.z };
		json["scale"] = { m_scale.x, m_scale.y, m_scale.z };
		return json;
	}

	TransformComponent* TransformComponent::deserialize(const nlohmann::json& json, Entity entity) {
		EntityComponentSystem& ecs = SceneManager::getActiveScene()->getECS();
		TransformComponent* comp = ecs.addComponent<TransformComponent>(entity);
		
		comp->m_position = { json["position"][0], json["position"][1], json["position"][2] };
		comp->m_rotation = { json["rotation"][0], json["rotation"][1], json["rotation"][2] };
		comp->m_scale = { json["scale"][0], json["scale"][1], json["scale"][2] };

		return comp;
	}
}