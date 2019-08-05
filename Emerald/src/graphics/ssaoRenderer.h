#pragma once

class SSAORenderer {
public:
	const int KERNELCOUNT = 64;

	float m_radius = 0.5f;
	float m_bias = 0.025f;
	int m_power = 1;

	SSAORenderer(uint width, uint height);
	~SSAORenderer();

	void Render(GBuffer* gBuffer);
	Texture* GetTexture() { return m_textureBlur; }
	Texture* GetRawTexture() { return m_texture; }

	void OnImGui() {
		ImGui::SliderFloat("bias", &m_radius, 0, 25);
		ImGui::SliderFloat("radius", &m_bias, 0, 25);
		ImGui::SliderInt("power", &m_power, 0, 64);
	}

private:
	Ref<FrameBuffer> m_fbo;
	Ref<FrameBuffer> m_fboBlur;
	Ref<Texture> m_texture;
	Ref<Texture> m_textureBlur;
	Ref<Texture> m_noiseTexture;
	Ref<Shader> m_shader;
	Ref<Shader> m_shaderBlur;
	Ref<Mesh> m_quad;

	vector<Vector3> m_kernels;
};