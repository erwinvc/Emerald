#pragma once

enum class ShadowType {
	DIRECTIONAL,
	OMNIDIRECTIONAL
};

class DirectionalShadow {
private:
	const uint SIZE = 1024;
	Shader* m_shaderChunk;
	Shader* m_shader;
	FrameBuffer* m_fbo;
	Texture* m_texture;

public:
	glm::mat4 m_lightSpaceMatrix;
	float m_nearPlane = -130.0f, m_farPlane = 55.0f;
	float m_size = 48.0f;
	
	DirectionalShadow();

	void Draw(HDRPipeline* pipeline, glm::vec3 lightpos);

	void OnImGui();
	
	inline Texture* GetTexture() { return m_texture; }
};