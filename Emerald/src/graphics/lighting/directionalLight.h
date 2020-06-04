#pragma once

class DirectionalLight {
private:
	float m_time = 0;
	
public:
	Color color = Color(1.0f, 0.95f, 0.95f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.1f, Math::PI);
	float multiplier = 4.0f;

	glm::vec3 GetDirection();

	//This should not be here...
	void SetTime(uint64 time, float partialTicks);
	
	void OnImGui();
	
	inline Color GetColor() { return color * multiplier; }
};