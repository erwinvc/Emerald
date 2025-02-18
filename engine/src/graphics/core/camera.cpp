#include "eepch.h"
#include "camera.h"
#include "glm/gtx/quaternion.hpp"

namespace emerald {
	void Camera::setViewportSize(uint32_t width, uint32_t height) {
		switch(m_projectionType){
			case ProjectionType::PERSPECTIVE:
				m_projectionMatrix = glm::perspectiveFov(glm::radians(m_fov), (float)width, (float)height, m_nearClip, m_farClip);
				break;
			case ProjectionType::ORTHOGRAPHIC:
				float aspect = (float)width / (float)height;
				float width = m_orthographicSize * aspect;
				float height = m_orthographicSize;
				m_projectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f);
				break;
		}
	}

	void Camera::setPosition(const glm::vec3& position) {
		m_position = position;
		updateViewMatrix();
	}

	void Camera::setRotation(const glm::vec3& rotation) {
		m_rotation = rotation;
		updateViewMatrix();
	}

	void Camera::updateViewMatrix() {
		m_viewMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(getOrientation());
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

	glm::quat Camera::getOrientation() const {
		return glm::quat(glm::vec3(-m_rotation.x, -m_rotation.y, 0.0f));
	}

	glm::vec3 Camera::forward() const {
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::right() const {
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::up() const {
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}
}