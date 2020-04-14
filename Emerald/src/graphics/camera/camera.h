#pragma once

struct CornerRayPositions {
	union {
		struct {
			glm::vec3 TL;
			glm::vec3 TR;
			glm::vec3 BR;
			glm::vec3 BL;
		};
		glm::vec3 corners[4];
	};

	CornerRayPositions() {}
};

class Camera {
private:
	glm::mat4 m_projectionMatrix;
	glm::mat4 m_viewMatrix;
	float m_FOV;
	float m_nearPlane;
	float m_farPlane;
	glm::vec4 m_viewPort;

protected:
	void UpdateViewMatrix() {
		m_viewMatrix = glm::translate(glm::mat4(1.0f), transform.m_position) * glm::toMat4(transform.GetOrientation());
		m_viewMatrix = glm::inverse(m_viewMatrix);
	}

public:
	Transform transform;

	Camera(float fov, float nearPlane, float farPlane) { SetProjectionMatrix(fov, nearPlane, farPlane); UpdateViewMatrix(); }

	virtual ~Camera() {}
	virtual void Update(const TimeStep& time) {}
	virtual void FixedUpdate() {}

	void UpdateProjectionMatrix();
	void SetProjectionMatrix(float fov, float nearPlane, float farPlane) {
		m_FOV = fov;
		m_nearPlane = nearPlane;
		m_farPlane = farPlane;
		UpdateProjectionMatrix();
	}

	void SetViewport(uint x, uint y, uint width, uint height) {
		m_viewPort.x = (float)x;
		m_viewPort.y = (float)y;
		m_viewPort.z = (float)width;
		m_viewPort.w = (float)height;
		UpdateProjectionMatrix();
	}


	inline glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	inline glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	inline glm::vec4 GetViewport() const { return m_viewPort; }
	inline float GetFOV() const { return m_FOV; }
	inline float GetNear() const { return m_nearPlane; }
	inline float GetFar() const { return m_farPlane; }

	virtual void OnImGui() {
		ImGui::InputFloat3("Position", (float*)&transform.m_position);
		ImGui::InputFloat3("Rotation", (float*)&transform.m_rotation);
		if (ImGui::InputFloat("Near", &m_nearPlane, 0.000001f, 10000.0f)) {
			UpdateProjectionMatrix();
		}

		if (ImGui::InputFloat("Far", &m_farPlane, 0.000001f, 10000.0f)) {
			UpdateProjectionMatrix();
		}

		if (ImGui::SliderFloat("FOV", &m_FOV, 0, 180)) {
			UpdateProjectionMatrix();
		}
	}

	static CornerRayPositions GetCornerRays(float offset) {
		glm::vec3& ray1 = GroundRaycast::GetScreenPosition(glm::vec2(0.0f + offset, 0.0f + offset));
		glm::vec3& ray2 = GroundRaycast::GetScreenPosition(glm::vec2(1.0f - offset, 0.0f + offset));
		glm::vec3& ray3 = GroundRaycast::GetScreenPosition(glm::vec2(1.0f - offset, 1.0f - offset));
		glm::vec3& ray4 = GroundRaycast::GetScreenPosition(glm::vec2(0.0f + offset, 1.0f - offset));
		CornerRayPositions positions;

		positions.TL = GroundRaycast::GetGroundPosition(ray1, 0);
		positions.TR = GroundRaycast::GetGroundPosition(ray2, 0);
		positions.BR = GroundRaycast::GetGroundPosition(ray3, 0);
		positions.BL = GroundRaycast::GetGroundPosition(ray4, 0);
		return positions;
	}
};