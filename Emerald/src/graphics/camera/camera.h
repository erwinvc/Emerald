#pragma once

class Camera
{
public:
	Vector3 m_position;
	Vector3 m_rotation;

	Camera() {
		m_position = Vector3();
		m_rotation = Vector3();
	}
	virtual ~Camera() {}

	Matrix4 GetViewMatrix() {
		Matrix4 transform(1);
        transform *= Matrix4::Rotate(m_rotation.x, Vector3::XAxis());
        transform *= Matrix4::Rotate(m_rotation.y, Vector3::YAxis());
        transform *= Matrix4::Rotate(m_rotation.z, Vector3::ZAxis());
		transform *= Matrix4::Translate(-m_position);
		return transform;
	}
};