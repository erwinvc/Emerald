#pragma once

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

	vector<Vector3> m_kernels;
public:
	bool m_enabled = true;

	const int KERNELCOUNT = 64;

	float m_radius = 0.5f;
	float m_bias = 0.025f;
	int m_power = 1;
	int m_sampleCount = 32;

	SSAORenderer();
	~SSAORenderer();

	void Render(GBuffer* gBuffer);

	Texture* GetTexture();
	Texture* GetRawTexture() { return m_texture; }

	void OnImGui() {
		ImGui::SliderFloat("Bias", &m_radius, 0, 1);
		ImGui::SliderFloat("Radius", &m_bias, 0, 1);
		ImGui::SliderInt("Power", &m_power, 0, 64);
		ImGui::SliderInt("Sample count", &m_sampleCount, 2, 64);
	}
};