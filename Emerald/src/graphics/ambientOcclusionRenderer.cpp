#include "stdafx.h"
AmbientOcclusionRenderer::~AmbientOcclusionRenderer() {
}

AmbientOcclusionRenderer::AmbientOcclusionRenderer() {
	m_fbo = GetFrameBufferManager()->Create("SSAO", FBOScale::FULL);
	m_fboBlur = GetFrameBufferManager()->Create("SSAOBlur", FBOScale::FULL);
	m_texture = m_fbo->AddBuffer("SSAO", TextureParameters(INT_RGB, DATA_RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	m_textureBlur = m_fboBlur->AddBuffer("SSAOBlur", TextureParameters(INT_RGB, DATA_RGB, NEAREST, CLAMP_TO_EDGE, T_FLOAT));

	//SSAO
	m_ssaoShader = GetShaderManager()->Get("SSAO");
	m_hbaoShader = GetShaderManager()->Get("HBAO");
	m_ssaoBlurShader = GetShaderManager()->Get("SSAOBlur");

	for (int i = 0; i < KERNELCOUNT; ++i) {
		glm::vec3 sample(Random::Float(1.0f) * 2.0f - 1.0f, Random::Float(1.0f) * 2.0f - 1.0f, Random::Float(1.0f));
		sample = glm::normalize(sample) * Random::Float(1.0f);
		float scale = (float)i / KERNELCOUNT;

		scale = Math::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		m_ssaoKernels.push_back(sample);
	}

	//SSAO noise
	vector<glm::vec3> ssaoNoise;
	for (int i = 0; i < 16; i++) {
		ssaoNoise.push_back(glm::vec3(Random::Float(1.0f) * 2.0f - 1.0f, Random::Float(1.0f) * 2.0f - 1.0f, 0.0f));
	}
	m_noiseTexture = NEW(Texture(4, 4, (byte*)ssaoNoise.data(), false, TextureParameters(INT_RGB32, DATA_RGB, NEAREST, REPEAT, T_FLOAT)));

	m_quad = MeshGenerator::Quad();

	m_ssaoShader->Bind();
	m_ssaoShader->Set("_Samples", m_ssaoKernels.data(), m_ssaoKernels.size());

	m_ssaoMaterial = GetMaterialManager()->Create("SSAO", m_ssaoShader);
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackGBuffer());
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackTexture("_Noise", m_noiseTexture, 4));
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_Radius", &m_ssaoRadius));
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_Bias", &m_ssaoBias));
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_Power", &m_ssaoPower));
	m_ssaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_SampleCount", &m_ssaoSampleCount));

	m_ssaoBlurMaterial = GetMaterialManager()->Create("SSAOBlur", m_ssaoBlurShader);
	m_ssaoBlurMaterial->AddOnBindCallback(new MaterialCallbackTexture("_SSAO", m_texture, 0));

	//HBAO
	m_hbaoMaterial = GetMaterialManager()->Create("HBAO", m_hbaoShader);
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackGBuffer());
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_Power", &m_hbaoPower));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_LumInfluence", &m_hbaoLumInfluence));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_AngleBias", &m_hbaoAngleBias));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_AttenuationScale", &m_hbaoAttenuationScale));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_SampleDirections", &m_hbaoSampleDirections));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_SampleSteps", &m_hbaoSampleSteps));
	m_hbaoMaterial->AddOnBindCallback(new MaterialCallbackPtr("_SampleRadius", &m_hbaoSampleRadius));

	LOG("[~bRenderer~x] AO renderer initialized");
}

void AmbientOcclusionRenderer::DrawSSAO(HDRPipeline* pipeline) {
	GL(glFrontFace(GL_CW));

	m_fbo->Bind();
	m_fbo->Clear();

	m_ssaoMaterial->Bind();
	m_quad->Draw();
	m_fbo->Unbind();

	m_fboBlur->Bind();
	m_ssaoBlurMaterial->Bind();
	m_quad->Draw();
	m_fboBlur->Unbind();

	GL(glFrontFace(GL_CCW));
}
void AmbientOcclusionRenderer::DrawHBAO(HDRPipeline* pipeline) {
	GL(glFrontFace(GL_CW));

	m_fbo->Bind();
	m_fbo->Clear();

	m_hbaoMaterial->Bind();
	m_quad->Draw();
	m_fbo->Unbind();

	GL(glFrontFace(GL_CCW));
}

void AmbientOcclusionRenderer::Draw(HDRPipeline* pipeline) {
	if (!m_enabled) return;
	switch (m_aoType) {
		case AmbientOcclusionType::SSAO:
			DrawSSAO(pipeline);
			break;
		case AmbientOcclusionType::HBAO:
			DrawHBAO(pipeline);
			break;
	}
}

void AmbientOcclusionRenderer::OnImGui() {
	static String_t scales[] = { "SSAO", "HBAO" };
	UI::Combo("AOType", (int*)&m_aoType, scales, NUMOF(scales));
	if (m_aoType == AmbientOcclusionType::SSAO) {
		UI::Float("Bias", &m_ssaoRadius, 0, 1);
		UI::Float("Radius", &m_ssaoBias, 0, 1);
		UI::Int("Power", &m_ssaoPower, 0, 32);
		UI::Int("Sample count", &m_ssaoSampleCount, 2, 64);
	} else {
		UI::Int("Power", &m_hbaoPower, 0, 32);
		UI::Float("Lum Influence", &m_hbaoLumInfluence, 0.0f, 1.0f);
		UI::Float("Angle Bias", &m_hbaoAngleBias, 0.0f, 1.0f);
		UI::Float("Attenuation Scale", &m_hbaoAttenuationScale, 0.0, 1.0);
		UI::Int("Sample Directions", &m_hbaoSampleDirections, 1, 32);
		UI::Int("Sample Steps", &m_hbaoSampleSteps, 1, 64);
		UI::Float("Sample Radius", &m_hbaoSampleRadius, 1, 32);
	}
}

Texture* AmbientOcclusionRenderer::GetTexture() {
	if (m_enabled) {
		if (m_aoType == AmbientOcclusionType::HBAO) return m_texture;
		else return m_textureBlur;
	}
	return  GetTextureManager()->GetWhiteTexture();
}