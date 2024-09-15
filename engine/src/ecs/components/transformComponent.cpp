#include "eepch.h"
#include "transformComponent.h"

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
}