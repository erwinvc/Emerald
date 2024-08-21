#include "eepch.h"
#include "camera.h"

namespace emerald {
	void Camera::setAspectRatio(float width, float height) {
		m_aspectRatio = width / height;
		updateProjection();
	}

	void Camera::setPosition(const glm::vec3& position) {
		m_position = position;
		updateView();
	}

	void Camera::setRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		updateView();
	}

	void Camera::updateProjection() {
		m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
	}

	void Camera::updateView() {
		glm::mat4 rotationMatrix = glm::yawPitchRoll(m_rotation.y, m_rotation.x, m_rotation.z);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -m_position);
		m_viewMatrix = rotationMatrix * translationMatrix;
	}
}