#pragma once

class DirectionalLight {
public:
    Color m_color = Color::White();
    Vector3 m_direction = Vector3(0.3f, 0.0f, 0.3f);

    void OnImGui() {
		if (ImGui::InputFloat3("Direction###1", (float*)&m_direction, -Math::PI, Math::PI));
		if (ImGui::SliderFloat3("Direction###2", (float*)&m_direction, -Math::PI, Math::PI));
		Matrix4 mat = Matrix4::Identity();
		mat *= Matrix4::Rotate(m_direction.x, Vector3::XAxis());
		mat *= Matrix4::Rotate(m_direction.y, Vector3::YAxis());
		mat *= Matrix4::Rotate(m_direction.z, Vector3::ZAxis());
		Vector3 a = mat * Vector3::Up();
		a.Normalize();
		ImGui::LabelText("euler", "%.3f %.3f %.3f", a.x, a.y, a.z);
        ImGui::ColorEdit3("Color", (float*)&m_color);
    }
};