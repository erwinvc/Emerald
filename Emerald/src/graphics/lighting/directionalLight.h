#pragma once

class DirectionalLight {
public:
    Color m_color = Color::White();
    Vector3 m_direction = Vector3(-0.7f, 0.3f, 0.1f);
    float m_ambient = 0.2f;
    float m_diffuse = 0.7f;
    float m_specular = 0.4f;

    void OnImGui() {
        ImGui::DragFloat("Specular", &m_specular, 0.01f, 0, 1);
        ImGui::DragFloat("Ambient", &m_ambient, 0.01f, 0, 1);
        ImGui::DragFloat("Diffuse", &m_diffuse, 0.01f, 0, 1);
        ImGui::ColorEdit3("Directional", (float*)&m_color);
    }
};