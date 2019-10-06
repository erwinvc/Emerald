#pragma once

struct CornerRayPositions {
	Vector3 c1;
	Vector3 c2;
	Vector3 c3;
	Vector3 c4;
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

	Camera(float fov, float nearPlane, float farPlane) { SetProjectionMatrix(fov, nearPlane, farPlane); }

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


	inline Matrix4& GetProjectionMatrix() { return m_projectionMatrix; }
	inline Matrix4& GetViewMatrix() { return m_viewMatrix; }
	inline float GetFOV() { return m_FOV; }
	inline float GetNear() { return m_nearPlane; }
	inline float GetFar() { return m_farPlane; }


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
		positions.c1 = GroundRaycast::GetGroundPosition(ray1, 0);
		positions.c2 = GroundRaycast::GetGroundPosition(ray2, 0);
		positions.c3 = GroundRaycast::GetGroundPosition(ray3, 0);
		positions.c4 = GroundRaycast::GetGroundPosition(ray4, 0);
		return positions;
	}
};