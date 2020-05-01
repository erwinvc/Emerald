#pragma once

class Mesh;
class SSAORenderer {
private:
	AssetRef<FrameBuffer> m_fbo;
	AssetRef<FrameBuffer> m_fboBlur;
	AssetRef<Texture> m_texture;
	AssetRef<Texture> m_textureBlur;
	AssetRef<Texture> m_noiseTexture;
	AssetRef<Shader> m_shader;
	AssetRef<Shader> m_shaderBlur;
	AssetRef<Mesh> m_quad;

	vector<glm::vec3> m_kernels;
public:
	bool m_enabled = true;

	const int KERNELCOUNT = 64;

	float m_radius = 1.0f;
	float m_bias = 0.09f;
	int m_power = 2;
	int m_sampleCount = 64;

	SSAORenderer();
	~SSAORenderer();

	void Render(GBuffer* gBuffer);

	Texture* GetTexture();
	Texture* GetRawTexture() { return m_texture; }

	void OnImGui() {
		UI::Float("Bias", &m_radius, 0, 1);
		UI::Float("Radius", &m_bias, 0, 1);
		UI::Int("Power", &m_power, 0, 64);
		UI::Int("Sample count", &m_sampleCount, 2, 64);
	}
};