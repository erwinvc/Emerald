#pragma once

class DirectionalLight {
private:
	float m_time = 0;
	Color m_color = Color(1.0f, 0.95f, 0.95f);
	//glm::vec3 m_rotation = glm::vec3(-0.5f, -0.5f, 1.0f);
public:
	glm::vec3 m_rotation = glm::vec3(0.0f, 0.1f, Math::PI);

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

		UI::Float("Time", &m_time, 0.0f, 1.0f);
		/*SetTime(m_time * 1200, 0);

		{
			int mod = ((int)(m_time * 1200) % 1200);
			float f = ((float)mod + 0) / 1200.0f - 0.25F;
			if (f < 0.0F) ++f;
			else if (f > 1.0F) --f;

			float f1 = 1.0F - (float)((Math::Cos(f * Math::PI) + 1.0f) / 2.0f);
			m_time = f + (f1 - f) / 3.0F;

			m_rotation.z = Math::HALF_PI + (m_time * Math::TWO_PI);

			float bottomColorFade = Math::Map(m_time, 0.0f, 0.15f, 0.0f, 1.0f);
			float topColorFade = Math::Map(m_time, 0.35f, 0.5f, 1.0f, 0.0f);
			float bottomBrightnessFade = Math::Map(m_time, 0.0f, 0.05f, 0.0f, 1.0f);
			float topBrightnessFade = Math::Map(m_time, 0.45f, 0.5f, 1.0f, 0.0f);

			if (m_time < 0.0f || m_time > 0.5f) m_color = Color::Black();
			else if (m_time > 0.15f && m_time < 0.35f) m_color = Color::White();
			else m_color = Color::Mix(Color(1.0f, 0.53f, 0.20f), Color::White(), m_time < 0.25f ? bottomColorFade : topColorFade);
			if (m_time > 0.0f && m_time < 0.5f) {
				if (m_time < 0.05f) m_color = m_color * bottomBrightnessFade;
				else if (m_time > 0.45f) m_color = m_color * topBrightnessFade;
			} else m_color = m_color * 0;
		}*/
		//UI::Float("Sun rotation", &m_rotation.z, Math::HALF_PI - 0.2f, Math::PI + Math::HALF_PI + 0.2f);
		UI::Vec3("Direction", &m_rotation, 0.0f, Math::TWO_PI);
		UI::Float("Multiplier", &m_multiplier, 0.0f, 10.0f);
		glm::vec3& direction = GetDirection();
		//ImGui::LabelText("euler", "%.3f %.3f %.3f", direction.x, direction.y, direction.z);
		UI::Color3("Color", &m_color);
	}

	
	void SetTime(uint64 time, float partialTicks) {
		int mod = (time % 1200);
		float f = ((float)mod + partialTicks) / 1200.0f - 0.25F;
		if (f < 0.0F) ++f;
		else if (f > 1.0F) --f;

		float f1 = 1.0F - (float)((Math::Cos(f * Math::PI) + 1.0f) / 2.0f);
		m_time = f + (f1 - f) / 3.0F;

		m_rotation.z = Math::HALF_PI + (m_time * Math::TWO_PI);

		float bottomColorFade = Math::Map(m_time, 0.0f, 0.15f, 0.0f, 1.0f);
		float topColorFade = Math::Map(m_time, 0.35f, 0.5f, 1.0f, 0.0f);
		float bottomBrightnessFade = Math::Map(m_time, 0.0f, 0.05f, 0.0f, 1.0f);
		float topBrightnessFade = Math::Map(m_time, 0.45f, 0.5f, 1.0f, 0.0f);

		if (m_time < 0.0f || m_time > 0.5f) m_color = Color::Black();
		else if (m_time > 0.15f && m_time < 0.35f) m_color = Color::White();
		else m_color = Color::Mix(Color(1.0f, 0.53f, 0.20f), Color::White(), m_time < 0.25f ? bottomColorFade : topColorFade);
		if (m_time > 0.0f && m_time < 0.5f) {
			if (m_time < 0.05f) m_color = m_color * bottomBrightnessFade;
			else if (m_time > 0.45f) m_color = m_color * topBrightnessFade;
		} else m_color = m_color * 0;
	}
};