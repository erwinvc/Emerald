#include "stdafx.h"
SSAORenderer::~SSAORenderer() {
	DELETE(m_noiseTexture);
	DELETE(m_shader);
	DELETE(m_shaderBlur);
	DELETE(m_quad);
}

SSAORenderer::SSAORenderer(uint width, uint height) :
	m_fbo(FrameBuffer::Create("SSAO", width, height)), m_fboBlur(FrameBuffer::Create("SSAOBlur", width, height)), m_texture(nullptr), m_textureBlur(nullptr), m_noiseTexture(nullptr), m_shader(nullptr), m_shaderBlur(nullptr), m_quad(nullptr){

	m_shader = NEW(Shader("SSAO", "src/shader/ssao.vert", "src/shader/ssao.frag"));
	m_shaderBlur = NEW(Shader("SSAO", "src/shader/ssao.vert", "src/shader/ssaoBlur.frag"));

	m_texture = NEW(Texture(width, height, TextureParameters(RGBA32, NEAREST, REPEAT, T_FLOAT)));
	m_fbo.AddColorBuffer(m_texture);

	m_textureBlur = NEW(Texture(width, height, TextureParameters(RGBA32, NEAREST, REPEAT, T_FLOAT)));
	m_fboBlur.AddColorBuffer(m_textureBlur);

	for (int i = 0; i < KERNELCOUNT; ++i) {
		Vector3 sample(Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f));
		sample = sample.Normalize() * Math::RandomFloat(1.0f);
		float scale = float(i) / KERNELCOUNT;

		scale = Math::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_kernels.push_back(sample);
	}

	//SSAO noise
	vector<Color> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++) {
		ssaoNoise.push_back(Color(Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f) * 2.0f - 1.0f, 0.0f, 1));
	}
	m_noiseTexture = NEW(Texture(4, 4, (byte*)ssaoNoise.data(), TextureParameters(RGBA32, NEAREST, REPEAT, T_FLOAT)));

	m_quad = MeshGenerator::Quad();

	m_shader->Bind();
	m_shader->Set("_GPosition", 0);
	m_shader->Set("_GNormal", 1);
	m_shader->Set("_Noise", 2);
	m_shader->Set("_NoiseScale", GetApplication()->GetWindow()->GetWidth() / 4, GetApplication()->GetWindow()->GetHeight() / 4);
	for (int i = 0; i < 64; i++) {
		m_shader->Set(Format_t("_Samples[%d]", i), m_kernels[i]);
	}

	LOG("[~bEEngine~x] SSAO initialized");
}

void SSAORenderer::Render(GBuffer* gBuffer) {
	GL(glDisable(GL_DEPTH_TEST));
	GL(glFrontFace(GL_CW));

	m_fbo.Bind();
	m_fbo.Clear();

	m_shader->Bind();
	m_shader->Set("_Radius", m_radius);
	m_shader->Set("_Bias", m_bias);
	m_shader->Set("_Power", m_power);
	m_shader->Set("_Projection", GetCamera()->GetProjectionMatrix());
	m_shader->Set("_View", GetCamera()->GetViewMatrix());

	gBuffer->m_positionTexture->Bind(0);
	gBuffer->m_normalTexture->Bind(1);
	m_noiseTexture->Bind(2);
	m_quad->Draw();
	m_fbo.Unbind();

	m_fboBlur.Bind();
	m_shaderBlur->Reload();
	m_shaderBlur->Bind();
	m_shaderBlur->Set("_SSAO", 0);
	m_texture->Bind();
	m_quad->Draw();
	m_fboBlur.Unbind();
}