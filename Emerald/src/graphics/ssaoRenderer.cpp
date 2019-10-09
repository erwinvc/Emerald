#include "stdafx.h"
SSAORenderer::~SSAORenderer() {
}

SSAORenderer::SSAORenderer(uint width, uint height) : m_texture(nullptr), m_textureBlur(nullptr), m_noiseTexture(nullptr), m_shader(nullptr), m_shaderBlur(nullptr), m_quad(nullptr) {

	m_fbo = GetFrameBufferManager()->Create("SSAO", FBOScale::HALF);
	m_fboBlur = GetFrameBufferManager()->Create("SSAOBlur", FBOScale::HALF);
	m_texture = m_fbo->AddBuffer("SSAO", TextureParameters(RED, RED, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_textureBlur = m_fboBlur->AddBuffer("SSAOBlur", TextureParameters(RED, RED, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	m_shader = GetShaderManager()->Get("SSAO");
	m_shaderBlur = GetShaderManager()->Get("SSAOBlur");

	for (int i = 0; i < KERNELCOUNT; ++i) {
		Vector3 sample(Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f));
		sample = sample.Normalized() * Math::RandomFloat(1.0f);
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
	m_noiseTexture = NEW(Texture(4, 4, (byte*)ssaoNoise.data(), false, TextureParameters(RGB16, RGB, NEAREST, REPEAT, T_FLOAT)));

	m_quad = MeshGenerator::Quad();

	m_shader->Bind();
	m_shader->Set("_GPosition", 0);
	m_shader->Set("_GNormal", 1);
	m_shader->Set("_Noise", 2);
	m_shader->Set("_Samples", m_kernels.data(), m_kernels.size());

	LOG("[~bRenderer~x] SSAO initialized");
}

void SSAORenderer::Render(GBuffer* gBuffer) {
	GL(glFrontFace(GL_CW));

	m_fbo->Bind();
	m_fbo->Clear();

	m_shader->Bind();
	m_shader->Set("_Radius", m_radius);
	m_shader->Set("_Bias", m_bias);
	m_shader->Set("_Power", m_power);
	m_shader->Set("_Projection", GetCamera()->GetProjectionMatrix());
	m_shader->Set("_View", GetCamera()->GetViewMatrix());
	m_shader->Set("_NoiseScale", Vector2(GetApp()->GetWidth() / 4.0f, GetApp()->GetHeight() / 4.0f));

	gBuffer->m_positionTexture->Bind(0);
	gBuffer->m_normalTexture->Bind(1);
	m_noiseTexture->Bind(2);
	m_quad->Draw();
	m_fbo->Unbind();

	m_fboBlur->Bind();
	m_shaderBlur->Bind();
	m_shaderBlur->Set("_SSAO", 0);
	m_texture->Bind();
	m_quad->Draw();
	m_fboBlur->Unbind();
	GL(glFrontFace(GL_CCW));
}