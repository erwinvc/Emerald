#pragma once

class Transform {
public:
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_size;

	Transform() : m_position(Vector3(0, 0, 0)), m_rotation(Vector3(0, 0, 0)), m_size(Vector3(1, 1, 1)) {}

	inline Matrix4 GetMatrix() const {
		return Matrix4::TRS(m_position, m_rotation, m_size);
	}

	void OnImGui() {
		ImGui::DragFloat3("Position", (float*)&m_position, 0.1f);
		ImGui::DragFloat3("Rotation", (float*)&m_rotation, 0.1f);
		ImGui::DragFloat3("Size", (float*)&m_size, 0.1f);
	}
};