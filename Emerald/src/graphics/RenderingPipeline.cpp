#include "stdafx.h"
#include <random>

void RenderingPipeline::Initialize() {

	//Deferred
	m_gBuffer = NEW(GBuffer());

	//#Dirty add proper shader asset loading
	m_directionalLightShader = GetShaderManager()->Get("Directional");
	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("_GMisc", 0);
	m_directionalLightShader->Set("_GAlbedo", 1);
	m_directionalLightShader->Set("_GNormal", 2);
	m_directionalLightShader->Set("_GPosition", 3);
	m_directionalLightShader->Set("_SSAO", 4);

	m_pointLightShader = GetShaderManager()->Get("Pointlight");
	m_pointLightShader->Bind();
	m_pointLightShader->Set("_GMisc", 0);
	m_pointLightShader->Set("_GAlbedo", 1);
	m_pointLightShader->Set("_GNormal", 2);
	m_pointLightShader->Set("_GPosition", 3);
	m_pointLightShader->Set("_SSAO", 4);
	m_gaussianShader = GetShaderManager()->Get("Gaussian");
	m_gaussianShader->Bind();
	m_gaussianShader->Set("_Bright", 0);

	m_emissionAmbientShader = GetShaderManager()->Get("EmissionAmbient");
	m_emissionAmbientShader->Bind();
	m_emissionAmbientShader->Set("_GMisc", 0);
	m_emissionAmbientShader->Set("_GAlbedo", 1);
	m_emissionAmbientShader->Set("_SSAO", 2);

	//HDR
	m_hdrShader = GetShaderManager()->Get("HDR");
	m_hdrShader->Bind();
	m_hdrShader->Set("_HDRBuffer", 0);
	m_hdrShader->Set("_HDRBloom", 1);

	m_hdrBuffer = GetFrameBufferManager()->Create("HDR", FBOScale::FULL, false);
	m_hdrTexture = m_hdrBuffer->AddBuffer("HDR", TextureParameters(RGB16, RGBA, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	m_hdrBrightTexture = m_hdrBuffer->AddBuffer("HDRBloom", TextureParameters(RGB, RGBA, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	GetFrameBufferManager()->SetSelectedTexture(m_hdrTexture);

	//Bloom
	m_pingPongFBO[0] = GetFrameBufferManager()->Create("PingPong1", FBOScale::ONEFIFTH, false);
	m_pingPongFBO[1] = GetFrameBufferManager()->Create("PingPong2", FBOScale::ONEFIFTH, false);
	m_pingPongTexture[0] = m_pingPongFBO[0]->AddBuffer("PingPong1", TextureParameters(RGB, RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));
	m_pingPongTexture[1] = m_pingPongFBO[1]->AddBuffer("PingPong1", TextureParameters(RGB, RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	//SSAO
	m_ssaoRenderer = NEW(SSAORenderer());

	m_freeCam = NEW(FreeCam(70, 0.1f, 90.0f));
	m_firstPersonCamera = NEW(FirstPersonCam(70, 0.1f, 90.0f));
	m_camera = m_freeCam;

	m_firstPersonCamera->m_position = Vector3(14, 0, -2);
	m_firstPersonCamera->m_rotation = Vector3(0, Math::PI, 0);
	m_camera->m_position = Vector3(0.0f, 0.5f, -1.5f);
	m_camera->m_rotation = Vector3(0.4f, Math::PI, 0.0f);

	//Final
	m_finalFBO = GetFrameBufferManager()->Create("Final", FBOScale::FULL, false);
	m_finalTexture = m_finalFBO->AddBuffer("Final", TextureParameters(RGB, RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	m_quad = MeshGenerator::Quad();

	m_initialized = true;
}

//void RenderingPipeline::Update(const TimeStep& time) {
	//m_lerpAmount = Math::Clamp(m_lerpAmount + time.GetSeconds(), 0.0f, 1.0f);
	//m_projectionMatrix = Matrix4::Lerp(m_projectionMatrix, m_perspective ? m_perspectiveMatrix : m_orthoMatrix, m_lerpAmount);
//}

void RenderingPipeline::PreGeometryRender() {
	glPatchParameteri(GL_PATCH_VERTICES, 3);

	GL(glEnable(GL_DEPTH_TEST));
	GL(glDepthMask(true));
	GL(glDisable(GL_BLEND));
	GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL(glEnable(GL_CULL_FACE));
	GL(glFrontFace(GL_CCW));

	//Draw to gBuffer
	m_gBuffer->Bind();
	m_gBuffer->Clear();

	GetPointlightRenderer()->Begin();

	if (m_wireFrame) {
		GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}
}

float roughness = 1;
float metalic = 0;
void RenderingPipeline::PostGeometryRender() {
	if (m_wireFrame) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));

	if (m_ssaoEnabled) m_ssaoRenderer->Render(m_gBuffer);

	//Draw to HDR
	m_hdrBuffer->Bind();
	m_hdrBuffer->Clear();

	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_ONE, GL_ONE));

	//Emission
	m_gBuffer->BindTextures();
	if (m_ssaoEnabled) m_ssaoRenderer->GetTexture()->Bind(2);
	else GetTextureManager()->GetWhiteTexture()->Bind(2);
	m_emissionAmbientShader->Bind();
	m_emissionAmbientShader->Set("_BloomFactor", m_bloomFactor);
	m_emissionAmbientShader->Set("_AmbientIntensity", m_ambientIntensity);
	m_directionalLightShader->Set("_SSAOEnabled", m_ssaoEnabled);
	m_quad->Bind();
	m_quad->Draw();

	//Draw directional light
	m_directionalLightShader->Bind();

	m_gBuffer->BindTextures();
	if (m_ssaoEnabled) m_ssaoRenderer->GetTexture()->Bind(4);
	else GetTextureManager()->GetWhiteTexture()->Bind(4);
	m_directionalLightShader->Set("_Color", m_directionalLight.GetColor());
	m_directionalLightShader->Set("_Directional", m_directionalLight.GetDirection());
	m_directionalLightShader->Set("_CameraPosition", m_camera->m_position);
	m_directionalLightShader->Set("_SSAOEnabled", m_ssaoEnabled);
	m_directionalLightShader->Set("_Roughness", roughness);
	m_directionalLightShader->Set("_Metallic", metallic);
	m_directionalLightShader->Set("_BloomFactor", m_bloomFactor);
	m_quad->Bind();
	m_quad->Draw();

	GL(glFrontFace(GL_CW));

	//Draw pointlights
	m_pointLightShader->Bind();
	m_gBuffer->BindTextures();

	if (m_ssaoEnabled) m_ssaoRenderer->GetTexture()->Bind(4);
	else GetTextureManager()->GetWhiteTexture()->Bind(4);
	m_pointLightShader->Set("projectionMatrix", m_camera->GetProjectionMatrix());
	m_pointLightShader->Set("viewMatrix", m_camera->GetViewMatrix());
	m_pointLightShader->Set("_CameraPosition", m_camera->m_position);
	m_pointLightShader->Set("_SSAOEnabled", m_ssaoEnabled);
	//m_pointLightShader->Set("_Roughness", roughness);
	//m_pointLightShader->Set("_Metallic", metallic);
	m_pointLightShader->Set("_BloomFactor", m_bloomFactor);
	GetPointlightRenderer()->End();
	GetPointlightRenderer()->Draw();

	m_hdrBuffer->Unbind();

	//Draw to screen
	GL(glDisable(GL_BLEND));

	bool horizontal = true, first_iteration = true;
	int amount = 8;
	m_gaussianShader->Bind();
	m_pingPongFBO[0]->Bind();
	m_pingPongFBO[0]->Clear();
	m_pingPongFBO[1]->Bind();
	m_pingPongFBO[1]->Clear();

	for (int i = 0; i < amount; i++) {
		m_pingPongFBO[horizontal]->Bind();

		m_gaussianShader->Set("horizontal", horizontal);

		if (first_iteration) m_hdrBrightTexture->Bind();
		else m_pingPongTexture[!horizontal]->Bind();
		m_quad->Draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}

	m_finalFBO->Bind();
	m_finalFBO->Clear();

	m_hdrShader->Bind();
	m_hdrShader->Set("_BloomMultiplier", m_bloomMultiplier);
	m_hdrShader->Set("_ApplyPostProcessing", m_applyPostProcessing);
	m_hdrShader->Set("_FXAA", m_FXAA);
	m_hdrShader->Set("_Gamma", m_gamma);
	m_hdrShader->Set("_Exposure", m_exposure);
	m_hdrShader->Set("_Tonemapping", m_selectedTonemapping);
	m_hdrShader->Set("_ScreenSize", Vector2(GetApp()->GetWidth<float>(), GetApp()->GetHeight<float>()));
	m_hdrShader->Set("_Chromatic", m_chromatic);
	m_hdrShader->Set("_Bloom", m_bloom);
	GetFrameBufferManager()->GetSelectedTexture()->Bind(0);

	m_pingPongTexture[1]->Bind(1);
	m_quad->Bind();
	m_quad->Draw();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_finalFBO->GetHandle());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_finalFBO->GetWidth(), m_finalFBO->GetHeight(), 0, 0, m_finalFBO->GetWidth(), m_finalFBO->GetHeight(), GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}

void RenderingPipeline::PreUIRender() {
	GLUtils::EnableBlending();
}

void RenderingPipeline::PostUIRender() {
	GLUtils::DisableBlending();
}

void RenderingPipeline::OnImGUI() {
	if (ImGui::BeginTabItem("Pipeline")) {
		const String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted", "Standard" };
		ImGui::DragFloat3("Directional", (float*)&m_directionalLight, 0.01f);
		if (ImGui::CollapsingHeader("HDR")) {
			ImGui::Checkbox("Post processing", &m_applyPostProcessing);
			ImGui::Checkbox("FXAA", &m_FXAA);

			if (ImGui::TreeNode("Bloom##1")) {
				ImGui::Checkbox("Bloom##2", &m_bloom);
				ImGui::SliderFloat("Bloom factor", &m_bloomFactor, 0, 2.0f);
				ImGui::SliderFloat("Bloom multiplier", &m_bloomMultiplier, 0, 5.0f);
				ImGui::SliderFloat("Chromatic", &m_chromatic, -0.01f, 0.01f);
				ImGui::TreePop();
				ImGui::Separator();
			}

			if (ImGui::TreeNode("Tonemapping")) {
				ImGui::SliderFloat("Gamma", &m_gamma, 0, 5);
				ImGui::SliderFloat("Exposure", &m_exposure, 0, 5);
				ImGui::Combo("Tonemapping", &m_selectedTonemapping, tonemapping, NUMOF(tonemapping));
				ImGui::TreePop();
				ImGui::Separator();
			}
		}
		if (ImGui::CollapsingHeader("Scene")) {
			if (ImGui::TreeNode("Lighting")) {
				ImGui::SliderFloat("Ambient", &m_ambientIntensity, 0, 1);
				ImGui::Text("Directional");
				m_directionalLight.OnImGui();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO")) {
				ImGui::Checkbox("Enabled", &m_ssaoEnabled);
				m_ssaoRenderer->OnImGui();
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Camera")) {
			if (ImGui::Button("Freecam")) {
				m_camera = m_freeCam;
				m_selectedCamera = 0;
				glfwSetInputMode(GetApp()->GetWindow()->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			if (ImGui::Button("First person")) {
				m_camera = m_firstPersonCamera;
				m_selectedCamera = 1;
			}
			m_camera->OnImGui();
		}

		if (ImGui::CollapsingHeader("Memory")) {
			GetMemory()->OnImGui();
		}

		if (ImGui::Checkbox("Wireframe", &m_wireFrame)) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		ImGui::SliderFloat("Roughness", &roughness, 0, 1);
		ImGui::SliderFloat("Metallic", &metallic, 0, 1);
		ImGui::EndTabItem();
	}
}


//#TODO properly resize fbos
void RenderingPipeline::OnResize(uint width, uint height) {
	if (!m_initialized) return;
	//m_gBuffer->Resize(width, height);
	// m_hdrBuffer->Resize(width, height);
	//m_ssaoRenderer->Resize(width, height);
	m_camera->SetViewport(0, 0, width, height);
}
