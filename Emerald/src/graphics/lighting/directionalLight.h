#pragma once

class DirectionalLight {
private:
	Color m_color = Color(1.0f, 0.95f, 0.95f);
	glm::vec3 m_rotation = glm::vec3(-0.5f, -0.5f, 1.0f);

public:
	float m_multiplier = 4.0f;

	Color GetColor() { return m_color * m_multiplier; }
	glm::vec3 GetDirection() {
		//glm::mat4 mat = glm::mat4::Identity();
		//mat *= glm::mat4::Rotate(m_rotation.x, glm::vec3::XAxis());
		//mat *= glm::mat4::Rotate(m_rotation.y, glm::vec3::YAxis());
		//mat *= glm::mat4::Rotate(m_rotation.z, glm::vec3::ZAxis());
		//glm::vec3 direction = mat * glm::vec3::Up();
		//direction.Normalize();
		//return direction;

		glm::mat4 mat = glm::mat4(1.0f);
		mat *= glm::rotate(mat, m_rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		mat *= glm::rotate(mat, m_rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		mat *= glm::rotate(mat, m_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 direction = mat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		return glm::normalize(direction);
	}

	void OnImGui() {
		UI::Header("Directional");
		//		ImGui::Property("Ambient", &m_ambientIntensity, 0.0f, 1.0f);

		UI::Vec3("Direction", &m_rotation, -3.0f, 3.0f);
		UI::Float("Multiplier", &m_multiplier, 0.0f, 10.0f);
		glm::vec3& direction = GetDirection();
		//ImGui::LabelText("euler", "%.3f %.3f %.3f", direction.x, direction.y, direction.z);
		UI::Color3("Color", &m_color);
	}
};