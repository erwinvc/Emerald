#pragma once

class DirectionalLight {
private:
	Color m_color = Color::White();
	glm::vec3 m_rotation = glm::vec3(0.3f, 0.0f, 0.3f);

public:
	float m_multiplier = 2;

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
		ImGui::InputFloat3("Direction###1", (float*)&m_rotation, -3, 3);
		ImGui::SliderFloat3("Direction###2", (float*)&m_rotation, -3, 3);
		ImGui::SliderFloat("Multiplier", &m_multiplier, 0, 10);
		glm::vec3& direction = GetDirection();
		ImGui::LabelText("euler", "%.3f %.3f %.3f", direction.x, direction.y, direction.z);
		ImGui::ColorEdit3("Color", (float*)&m_color);
	}
};