#pragma once

class SSAORenderer {
public:
	const int KERNELCOUNT = 64;

	float m_radius = 0.5f;
	float m_bias = 0.025f;
	int m_power = 1;

	SSAORenderer(uint width, uint height) { Initialize(width, height); }
	~SSAORenderer();

	void Render(GBuffer* gBuffer);
	Texture* GetTexture() { return m_textureBlur; }
	Texture* GetRawTexture() { return m_texture; }
private:
	FrameBuffer* m_fbo;
	FrameBuffer* m_fboBlur;
	Texture* m_texture;
	Texture* m_textureBlur;
	Texture* m_noiseTexture;
	Shader* m_shader;
	Shader* m_shaderBlur;
	Mesh* m_quad;

	vector<Vector3> m_kernels;

	void Initialize(uint width, uint height);
};