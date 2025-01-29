#include "eepch.h"
#include "transformComponent.h"
#include "../core/entityComponentSystem.h"
#include "../../scene/sceneManager.h"
#include "../core/ECSManager.h"

namespace emerald {
	COMPONENT_DEF(TransformComponent);

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

	void TransformComponent::deserialize(const nlohmann::json& json) {
		m_position = { json["position"][0], json["position"][1], json["position"][2] };
		m_rotation = { json["rotation"][0], json["rotation"][1], json["rotation"][2] };
		m_scale = { json["scale"][0], json["scale"][1], json["scale"][2] };
	}
}