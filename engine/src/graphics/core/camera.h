#pragma once
#include "utils/system/timestep.h"

namespace emerald {
	enum class ProjectionType {
		PERSPECTIVE,
		ORTHOGRAPHIC
	};
	class Camera : public RefCounted {
	public:
		Camera(float fov, float nearClip, float farClip, ProjectionType projectionType = ProjectionType::PERSPECTIVE)
			: m_fov(fov), m_nearClip(nearClip), m_farClip(farClip), m_projectionType(projectionType){
		}

		virtual ~Camera() = default;

		void setViewportSize(uint32_t width, uint32_t height);
		void setPosition(const glm::vec3& position);
		void setRotation(const glm::vec3& rotation);

		const glm::mat4& getViewMatrix() const { return m_viewMatrix; }
		const glm::mat4& getProjectionMatrix() const { return m_projectionMatrix; }
		const glm::vec3& getPosition() const { return m_position; }
		const glm::vec3& getRotation() const { return m_rotation; }

		glm::quat getOrientation() const;
		glm::vec3 forward() const;
		glm::vec3 right() const;
		glm::vec3 up() const;

	protected:
		float m_fov, m_nearClip, m_farClip;

		float m_orthographicSize = 10.0f;
		ProjectionType m_projectionType;

		glm::vec3 m_position = glm::vec3(0.0f);
		glm::vec3 m_rotation = glm::vec3(0.0f);
		glm::mat4 m_viewMatrix = glm::mat4(1.0f);
		glm::mat4 m_projectionMatrix = glm::mat4(1.0f);

		void updateViewMatrix();
	};
}