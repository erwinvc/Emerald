#include "stdafx.h"
SSAORenderer::~SSAORenderer() {
}

SSAORenderer::SSAORenderer() : m_texture(nullptr), m_textureBlur(nullptr), m_noiseTexture(nullptr), m_shader(nullptr), m_shaderBlur(nullptr), m_quad(nullptr) {
	m_fbo = GetFrameBufferManager()->Create("SSAO", FBOScale::FULL, false);
	m_fboBlur = GetFrameBufferManager()->Create("SSAOBlur", FBOScale::FULL, false);
	m_texture = m_fbo->AddBuffer("SSAO", TextureParameters(RGB, RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	m_textureBlur = m_fboBlur->AddBuffer("SSAOBlur", TextureParameters(RGB, RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));

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
	vector<Vector3> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++) {
		ssaoNoise.push_back(Vector3(Math::RandomFloat(1.0f) * 2.0f - 1.0f, Math::RandomFloat(1.0f) * 2.0f - 1.0f, 0.0f));
	}
	m_noiseTexture = NEW(Texture(4, 4, (byte*)ssaoNoise.data(), false, TextureParameters(RGB32, RGB, NEAREST, REPEAT, T_FLOAT)));

	m_quad = MeshGenerator::Quad();

	m_shader->Bind();
	m_shader->Set("_Depth", 0);
	m_shader->Set("_GAlbedo", 1);
	m_shader->Set("_GNormal", 2);
	m_shader->Set("_GMisc", 3);
	m_shader->Set("_Noise", 4);

	m_shader->Set("_Samples", m_kernels.data(), m_kernels.size());

	LOG("[~bRenderer~x] SSAO initialized");
}

void SSAORenderer::Render(GBuffer* gBuffer) {
	if (!m_enabled) return;
	GL(glFrontFace(GL_CW));

	m_fbo->Bind();
	m_fbo->Clear();

	m_shader->Bind();
	m_shader->Set("_Radius", m_radius);
	m_shader->Set("_Bias", m_bias);
	m_shader->Set("_Power", m_power);
	m_shader->Set("_SampleCount", m_sampleCount);
	m_shader->Set("_Projection", GetCamera()->GetProjectionMatrix());
	m_shader->Set("_View", GetCamera()->GetViewMatrix());
	m_shader->Set("_ScreenSize", Vector2((float)m_fbo->GetWidth() / 4.0f, (float)m_fbo->GetHeight() / 4.0f));
	m_shader->Set("_CameraPlanes", Vector2(GetCamera()->GetNear(), GetCamera()->GetFar()));
	gBuffer->BindTextures();
	m_noiseTexture->Bind(4);
	m_quad->Draw();
	m_fbo->Unbind();

	m_fboBlur->Bind();
	m_shaderBlur->Bind();
	m_shaderBlur->Set("_SSAO", 0);
	m_texture->Bind(0);
	m_quad->Draw();
	m_fboBlur->Unbind();
	GL(glFrontFace(GL_CCW));
}

Texture* SSAORenderer::GetTexture() { return m_enabled ? m_textureBlur : GetTextureManager()->GetWhiteTexture(); }