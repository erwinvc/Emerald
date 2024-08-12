#pragma once
#include <util/timestep.h>

namespace emerald{
class Camera {
protected:
	glm::mat4 m_Proj = glm::identity<glm::mat4>();
		glm::mat4 m_View = glm::identity<glm::mat4>();
		glm::vec3 m_Position = glm::zero<glm::vec3>();
		glm::vec3 m_Rotation = glm::zero<glm::vec3>();
		glm::vec3 m_FocalPoint = glm::zero<glm::vec3>();

public:
	Camera(const glm::mat4& projection) {}
	~Camera() {}

	inline const glm::mat4& GetProjectionMatrix() const { return m_Proj; }
	inline const glm::mat4& GetViewMatrix() const { return m_View; }

	inline void SetProjectionMatrix(const glm::mat4& projectionMatrix) { m_Proj = projectionMatrix; }
	inline void SetViewMatrix(const glm::mat4& ViewMatrix) { m_View = ViewMatrix; }

	virtual void OnUpdate(Timestep timeStep);

	virtual void OnFocus() {}

	inline const glm::vec3& GetPosition() const { return m_Position; }
	inline void SetPosition(const glm::vec3& position) { m_Position = position; }

	inline const glm::vec3& GetRotation() const { return m_Rotation; }
	inline void SetRotation(const glm::vec3& rotation) { m_Rotation = rotation; }

	inline void Translate(const glm::vec3& translation) { m_Position += translation; }
	inline void Rotate(const glm::vec3& rotation) { m_Rotation += rotation; }

	inline void Translate(float x, float y, float z) { m_Position += glm::vec3(x, y, z); }
	inline void Rotate(float x, float y, float z) { m_Rotation += glm::vec3(x, y, z); }

	inline const glm::vec3& GetFocalPoint() const { return m_FocalPoint; }

	inline const glm::mat4& GetViewMatrix() { return m_View; }
};
}