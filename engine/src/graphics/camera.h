#pragma once
#include <util/timestep.h>

namespace emerald {
	class Camera : public RefCounted {
	public:
		Camera(float fov, float nearClip, float farClip)
			: m_fov(fov), m_nearClip(nearClip), m_farClip(farClip), m_aspectRatio(1.0f) {
			updateProjection();
		}

		virtual ~Camera() = default;

		void setAspectRatio(float width, float height);
		void setPosition(const glm::vec3& position);
		void setRotation(const glm::vec3& rotation);

		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::vec3& getPosition() const { return m_position; }
		const glm::vec3& getRotation() const { return m_rotation; }


	protected:
		float m_fov, m_nearClip, m_farClip, m_aspectRatio;
		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f);
		glm::mat4 m_viewMatrix = glm::mat4(1.0f);
		glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

		void updateProjection();
		void updateView();
	};
}