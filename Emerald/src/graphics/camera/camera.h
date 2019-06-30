#pragma once

class Camera {
private:
	//m_perspectiveMatrix = Matrix4::Perspective(70, aspect, 0.01f, 1000.0f);
	//m_orthoMatrix = Matrix4::Orthographic(-5 * aspect, 5 * aspect, -5, 5, -500.0f, 500.0f);
	Matrix4 m_projectionMatrix;
	Matrix4 m_viewMatrix;

protected:
	void UpdateViewMatrix() {
		 Matrix4 transform= Matrix4::Translate(Vector3(0, 0, 1));
		transform *= Matrix4::Rotate(m_rotation.x, Vector3::XAxis());
		transform *= Matrix4::Rotate(m_rotation.y, Vector3::YAxis());
		transform *= Matrix4::Rotate(m_rotation.z, Vector3::ZAxis());
		transform *= Matrix4::Translate(-m_position);
		m_viewMatrix = transform;
	}

public:
	Vector3 m_position = Vector3();
	Vector3 m_rotation = Vector3();

	Camera(float fov, float aspectRatio, float nearPlane, float farPlane) { SetProjectionMatrix(fov, aspectRatio, nearPlane, farPlane); }

	virtual ~Camera() {}
	virtual void Update(const TimeStep& time) {}
	virtual void FixedUpdate() {}

	void SetProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {
		m_projectionMatrix = Matrix4::Perspective(fov, aspectRatio, nearPlane, farPlane);
	}



	Matrix4& GetProjectionMatrix() { return m_projectionMatrix; }
	Matrix4& GetViewMatrix() { return m_viewMatrix; }

	void OnImGui() {
		ImGui::InputFloat3("Position", (float*)&m_position);
		ImGui::InputFloat3("Rotation", (float*)&m_rotation);
	}
};