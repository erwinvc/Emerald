#pragma once

struct CornerRayPositions {
	union {
		struct {
			Vector3 TL;
			Vector3 TR;
			Vector3 BR;
			Vector3 BL;
		};
		Vector3 corners[4];
	};

	CornerRayPositions() {}
};

class Camera {
private:
	Matrix4 m_projectionMatrix;
	Matrix4 m_viewMatrix;
	float m_FOV;
	float m_nearPlane;
	float m_farPlane;
	Vector4 m_viewPort;

protected:
	void UpdateViewMatrix() {
		Matrix4 transform = Matrix4::Translate(Vector3(0, 0, 1));
		transform *= Matrix4::Rotate(m_rotation.x, Vector3::XAxis());
		transform *= Matrix4::Rotate(m_rotation.y, Vector3::YAxis());
		transform *= Matrix4::Rotate(m_rotation.z, Vector3::ZAxis());
		transform *= Matrix4::Translate(-m_position);
		m_viewMatrix = transform;
	}

public:
	Vector3 m_position = Vector3();
	Vector3 m_rotation = Vector3();

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


	inline Matrix4 GetProjectionMatrix() const { return m_projectionMatrix; }
	inline Matrix4 GetViewMatrix() const { return m_viewMatrix; }
	inline Vector4 GetViewport() const { return m_viewPort; }
	inline float GetFOV() const { return m_FOV; }
	inline float GetNear() const { return m_nearPlane; }
	inline float GetFar() const { return m_farPlane; }

	virtual void OnImGui() {
		ImGui::InputFloat3("Position", (float*)&m_position);
		ImGui::InputFloat3("Rotation", (float*)&m_rotation);
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
		Vector3& ray1 = GroundRaycast::GetScreenPosition(Vector2(0.0f + offset, 0.0f + offset));
		Vector3& ray2 = GroundRaycast::GetScreenPosition(Vector2(1.0f - offset, 0.0f + offset));
		Vector3& ray3 = GroundRaycast::GetScreenPosition(Vector2(1.0f - offset, 1.0f - offset));
		Vector3& ray4 = GroundRaycast::GetScreenPosition(Vector2(0.0f + offset, 1.0f - offset));
		CornerRayPositions positions;

		positions.TL = GroundRaycast::GetGroundPosition(ray1, 0);
		positions.TR = GroundRaycast::GetGroundPosition(ray2, 0);
		positions.BR = GroundRaycast::GetGroundPosition(ray3, 0);
		positions.BL = GroundRaycast::GetGroundPosition(ray4, 0);
		return positions;
	}
};