#pragma once

struct CornerRayPositions {
	Vector3 TL;
	Vector3 TR;
	Vector3 BR;
	Vector3 BL;
};

class Camera {
private:
	Matrix4 m_projectionMatrix;
	Matrix4 m_viewMatrix;
	float m_FOV;
	float m_nearPlane;
	float m_farPlane;

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


	inline Matrix4 GetProjectionMatrix() const { return m_projectionMatrix; }
	inline Matrix4 GetViewMatrix() const { return m_viewMatrix; }
	inline float GetFOV() const { return m_FOV; }
	inline float GetNear() const { return m_nearPlane; }
	inline float GetFar() const { return m_farPlane; }


	virtual void OnImGui() {
		ImGui::InputFloat3("Position", (float*)&m_position);
		ImGui::InputFloat3("Rotation", (float*)&m_rotation);
	}

	static CornerRayPositions GetCornerRays() {
		Vector3& ray1 = GroundRaycast::GetScreenPosition(Vector2(0.0f, 0.0f));
		Vector3& ray2 = GroundRaycast::GetScreenPosition(Vector2(1.0f, 0.0f));
		Vector3& ray3 = GroundRaycast::GetScreenPosition(Vector2(1.0f, 1.0f));
		Vector3& ray4 = GroundRaycast::GetScreenPosition(Vector2(0.0f, 1.0f));
		CornerRayPositions positions;
		positions.TL = GroundRaycast::GetGroundPosition(ray1, 0);
		positions.TR = GroundRaycast::GetGroundPosition(ray2, 0);
		positions.BR = GroundRaycast::GetGroundPosition(ray3, 0);
		positions.BL = GroundRaycast::GetGroundPosition(ray4, 0);
		return positions;
	}
};