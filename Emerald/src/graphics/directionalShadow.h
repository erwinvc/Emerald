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
	DirectionalShadow() {
		m_shaderChunk = GetShaderManager()->Get("DirectionalShadowChunk");
		m_shader = GetShaderManager()->Get("DirectionalShadow");
		m_fbo = GetFrameBufferManager()->Create("DirectionalShadow", SIZE, SIZE, Color(1.0f, 1.0f, 0.0f, 0.0f));
		m_fbo->AddBuffer("Depth", TextureParameters(INT_DEPTH, DATA_DEPTH, LINEAR, REPEAT, T_UNSIGNED_BYTE), FBOAttachment::DEPTH);
		m_texture = m_fbo->AddBuffer("DirectionalShadow", TextureParameters(INT_RG32, DATA_RGBA, LINEAR, REPEAT, T_FLOAT), FBOAttachment::COLOR);

		//m_fbo->SetDrawAndReadBuffersToNone();
	}

	void Draw(HDRPipeline* pipeline, glm::vec3 lightpos);

	Texture* GetTexture() { return m_texture; }

	void OnImGui()
	{
		UI::Begin();
		UI::Float("Far", &m_farPlane, 2.0f, 512.0f);
		UI::Float("Near", &m_nearPlane, -200.0f, 1.0f);
		UI::Float("Size", &m_size, 1.0f, 64.0f);
		UI::End();
	}
};