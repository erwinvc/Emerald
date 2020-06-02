#include "stdafx.h"

BloomRenderer::BloomRenderer() {
	m_bloomFBO = GetFrameBufferManager()->Create("Bloom", FBOScale::FULL);

	m_bloomTexture = m_bloomFBO->AddBuffer("Bloom", TextureParameters(INT_RGB16, DATA_RGB, LINEAR, CLAMP_TO_EDGE, T_FLOAT));

	m_pingPongFBO[0] = GetFrameBufferManager()->Create("PingPong1", FBOScale::HALF);
	m_pingPongFBO[1] = GetFrameBufferManager()->Create("PingPong2", FBOScale::HALF);
	m_pingPongTexture[0] = m_pingPongFBO[0]->AddBuffer("PingPong1", TextureParameters(INT_RGB, DATA_RGB, LINEAR, CLAMP_TO_EDGE, T_FLOAT));
	m_pingPongTexture[1] = m_pingPongFBO[1]->AddBuffer("PingPong1", TextureParameters(INT_RGB, DATA_RGB, LINEAR, CLAMP_TO_EDGE, T_FLOAT));

	m_bloomShader = GetShaderManager()->Get("Bloom");
	m_bloomShader->Bind();
	m_bloomShader->Set("_HDR", 0);

	m_gaussianShader = GetShaderManager()->Get("Gaussian");
	m_gaussianShader->Bind();
	m_gaussianShader->Set("_Texture", 0);

	m_quad = MeshGenerator::Quad();
}

void BloomRenderer::Draw(HDRPipeline* pipeline) {
	auto& pBloom = GetProfiler()->StartGL(ProfilerDataType::Bloom);
	if (m_enabled) {
		m_bloomFBO->Bind();
		m_bloomShader->Bind();
		pipeline->GetHDRTexture()->Bind(0);
		m_quad->Bind();
		m_quad->Draw();

		bool horizontal = true;
		bool firstIteration = true;

		m_gaussianShader->Bind();
		m_pingPongFBO[0]->Bind();
		m_pingPongFBO[0]->Clear();
		m_pingPongFBO[1]->Bind();
		m_pingPongFBO[1]->Clear();

		for (int i = 0; i < m_gaussianIterations; i++) {
			m_pingPongFBO[horizontal]->Bind();
			m_gaussianShader->Set("_Horizontal", horizontal);

			if (firstIteration) m_bloomTexture->Bind();
			else m_pingPongTexture[!horizontal]->Bind();
			m_quad->Draw();

			horizontal ^= true;
			if (firstIteration) firstIteration = false;
		}
	}
	pBloom.End();
}