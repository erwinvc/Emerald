#include "stdafx.h"

glm::vec3 DirectionalLight::GetDirection() {
	glm::mat4 mat = glm::mat4(1.0f);
	mat *= glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	mat *= glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	mat *= glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::vec3 direction = mat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
	return glm::normalize(direction);
}

void DirectionalLight::SetTime(uint64 time, float partialTicks) {
	int mod = (time % 12000);
	float f = ((float)mod + partialTicks) / 12000.0f - 0.25F;
	if (f < 0.0F) ++f;
	else if (f > 1.0F) --f;

	float f1 = 1.0F - (float)((Math::Cos(f * Math::PI) + 1.0f) / 2.0f);
	m_time = f + (f1 - f) / 3.0F;

	rotation.z = Math::HALF_PI + (m_time * Math::TWO_PI);

	float bottomColorFade = Math::Map(m_time, 0.0f, 0.15f, 0.0f, 1.0f);
	float topColorFade = Math::Map(m_time, 0.35f, 0.5f, 1.0f, 0.0f);
	float bottomBrightnessFade = Math::Map(m_time, 0.0f, 0.05f, 0.0f, 1.0f);
	float topBrightnessFade = Math::Map(m_time, 0.45f, 0.5f, 1.0f, 0.0f);

	if (m_time < 0.0f || m_time > 0.5f) color = Color::Black();
	else if (m_time > 0.15f && m_time < 0.35f) color = Color::White();
	else color = Color::Mix(Color(1.0f, 0.53f, 0.20f), Color::White(), m_time < 0.25f ? bottomColorFade : topColorFade);
	if (m_time > 0.0f && m_time < 0.5f) {
		if (m_time < 0.05f) color = color * bottomBrightnessFade;
		else if (m_time > 0.45f) color = color * topBrightnessFade;
	} else color = color * 0;
}

void DirectionalLight::OnImGui() {
	UI::Header("Directional");
	UI::Float("Time", &m_time, 0.0f, 1.0f);
	UI::Vec3("Direction", &rotation, 0.0f, Math::TWO_PI);
	UI::Float("Multiplier", &multiplier, 0.0f, 10.0f);
	glm::vec3& direction = GetDirection();
	UI::Color3("Color", &color);
}