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
	const int KERNELCOUNT = 64;

	float m_radius = 0.1f;
	float m_bias = 0.01f;
	int m_power = 1;

	SSAORenderer();
	~SSAORenderer();

	void Render(GBuffer* gBuffer);

	AssetRef<Texture> GetTexture() { return m_textureBlur; }
	AssetRef<Texture> GetRawTexture() { return m_texture; }

	void OnImGui() {
		ImGui::SliderFloat("bias", &m_radius, 0, 1);
		ImGui::SliderFloat("radius", &m_bias, 0, 1);
		ImGui::SliderInt("power", &m_power, 0, 64);
	}
};