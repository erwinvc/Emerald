#include "stdafx.h"
#include <random>

void HDRPipeline::Initialize() {

	//Deferred
	m_gBuffer = NEW(GBuffer());

	//#Dirty add proper shader asset loading
	m_directionalLightShader = GetShaderManager()->Get("Directional");
	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("_Depth", 0);
	m_directionalLightShader->Set("_GAlbedo", 1);
	m_directionalLightShader->Set("_GNormal", 2);
	m_directionalLightShader->Set("_GMisc", 3);
	m_directionalLightShader->Set("_SSAO", 4);

	m_pointLightShader = GetShaderManager()->Get("Pointlight");
	m_pointLightShader->Bind();
	m_pointLightShader->Set("_Depth", 0);
	m_pointLightShader->Set("_GAlbedo", 1);
	m_pointLightShader->Set("_GNormal", 2);
	m_pointLightShader->Set("_GMisc", 3);
	m_pointLightShader->Set("_SSAO", 4);

	m_gaussianShader = GetShaderManager()->Get("Gaussian");
	m_gaussianShader->Bind();
	m_gaussianShader->Set("_Bright", 0);

	m_emissionAmbientShader = GetShaderManager()->Get("EmissionAmbient");
	m_emissionAmbientShader->Bind();
	m_emissionAmbientShader->Set("_GMisc", 0);
	m_emissionAmbientShader->Set("_GAlbedo", 1);
	m_emissionAmbientShader->Set("_SSAO", 2);

	//Shadow
	m_shadowRenderer = NEW(ShadowRenderer(1024, 1024));

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

	//SSR
	m_ssrRenderer = NEW(SSRRenderer());

	m_freeCam = NEW(FreeCam(70, 0.1f, 1000.0f));
	m_freeCam->SetViewport(0, 0, 1920, 1080);
	m_firstPersonCamera = NEW(FirstPersonCam(70, 0.1f, 1000.0f));
	m_firstPersonCamera->SetViewport(0, 0, 1920, 1080);

	Camera::active = m_freeCam;

	m_firstPersonCamera->transform.m_position = glm::vec3(14, 0, -2);
	m_firstPersonCamera->transform.m_rotation = glm::vec3(0, Math::PI, 0);
	Camera::active->transform.m_position = glm::vec3(0.0f, 0.5f, -1.5f);
	Camera::active->transform.m_rotation = glm::vec3(0.4f, Math::PI, 0.0f);

	//Final
	m_finalFBO = GetFrameBufferManager()->Create("Final", FBOScale::FULL, false);
	m_finalTexture = m_finalFBO->AddBuffer("Final", TextureParameters(RGB, RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	m_quad = MeshGenerator::Quad();

	m_initialized = true;
}

void HDRPipeline::Render() {
	m_spriteRenderer->Begin();
	if (m_initialized) {
		PreGeometryRender();

		GetLineRenderer()->Begin();
		GetStateManager()->RenderGeometry(this);
		GetLineRenderer()->End();
		GetLineRenderer()->Draw();

		PostGeometryRender();
	} else GetStateManager()->RenderGeometry(this);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	GL(glViewport(0, 0, m_width, m_height));

	m_spriteRenderer->End();
	GLUtils::EnableBlending();
	m_spriteRenderer->Draw();
	GLUtils::DisableBlending();

	if (GetImGuiManager()->IsInitialized()) {
		GetImGuiManager()->Begin();
	
		if (ImGui::Begin("Emerald###Window", &m_ImGuiOpen, ImVec2(576, 680), -1)) {
			if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
				OnImGUI();
				GetStateManager()->OnStateImGUI();
				GetFrameBufferManager()->OnImGUI();
				GetShaderManager()->OnImGUI();
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
	
		GetStateManager()->OnImGUI();
		GetImGuiManager()->End();
	}
}

void HDRPipeline::PreShadowRender() {
	GL(glEnable(GL_DEPTH_TEST));
	GL(glDepthMask(true));
	GL(glDisable(GL_BLEND));
	GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL(glEnable(GL_CULL_FACE));
	GL(glFrontFace(GL_CCW));

	m_shadowRenderer->Begin(this);
}
void HDRPipeline::PostShadowRender() {
	m_shadowRenderer->End(this);
}
void HDRPipeline::PreGeometryRender() {
	//glPatchParameteri(GL_PATCH_VERTICES, 3);

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

void HDRPipeline::PostGeometryRender() {
	if (m_wireFrame) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));

	m_ssaoRenderer->Render(m_gBuffer);

	//Draw to HDR
	m_hdrBuffer->Bind();
	m_hdrBuffer->Clear();

	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_ONE, GL_ONE));

	//Emission
	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(2);
	m_emissionAmbientShader->Bind();
	m_emissionAmbientShader->Set("_BloomFactor", m_bloomFactor);
	m_emissionAmbientShader->Set("_AmbientIntensity", m_ambientIntensity);
	m_directionalLightShader->Set("_SSAOEnabled", m_ssaoRenderer->m_enabled);
	m_quad->Bind();
	m_quad->Draw();

	//Draw directional light
	m_directionalLightShader->Bind();

	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(4);
	m_directionalLightShader->Set("_Color", m_directionalLight.GetColor());
	m_directionalLightShader->Set("_Directional", m_directionalLight.GetDirection());
	m_directionalLightShader->Set("_CameraPosition", Camera::active->transform.m_position);
	m_directionalLightShader->Set("_SSAOEnabled", m_ssaoRenderer->m_enabled);
	m_directionalLightShader->Set("_Roughness", roughness);
	m_directionalLightShader->Set("_Metallic", metallic);
	m_directionalLightShader->Set("_BloomFactor", m_bloomFactor);
	m_directionalLightShader->Set("_Projection", Camera::active->GetProjectionMatrix());
	m_directionalLightShader->Set("_View", Camera::active->GetViewMatrix());
	m_quad->Bind();
	m_quad->Draw();

	GL(glFrontFace(GL_CW));

	//Draw pointlights
	m_pointLightShader->Bind();
	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(4);
	m_pointLightShader->Set("_Projection", Camera::active->GetProjectionMatrix());
	m_pointLightShader->Set("_View", Camera::active->GetViewMatrix());
	m_pointLightShader->Set("_CameraPosition", Camera::active->transform.m_position);
	m_pointLightShader->Set("_SSAOEnabled", m_ssaoRenderer->m_enabled);
	//m_pointLightShader->Set("_Roughness", roughness);
	//m_pointLightShader->Set("_Metallic", metallic);
	m_pointLightShader->Set("_BloomFactor", m_bloomFactor);
	GetPointlightRenderer()->End();
	GetPointlightRenderer()->Draw();

	m_hdrBuffer->Unbind();

	//Draw to screen
	GL(glDisable(GL_BLEND));

	m_ssrRenderer->Draw(this);

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
	m_hdrShader->Set("_ApplyPostProcessing", GetFrameBufferManager()->GetSelectedTexture() == m_hdrTexture && m_applyPostProcessing);
	m_hdrShader->Set("_FXAA", m_FXAA);
	m_hdrShader->Set("_Gamma", m_gamma);
	m_hdrShader->Set("_Exposure", m_exposure);
	m_hdrShader->Set("_Tonemapping", m_selectedTonemapping);
	m_hdrShader->Set("_ScreenSize", glm::vec2(m_width, m_height));
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

void HDRPipeline::OnImGUI() {
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
				ImGui::Checkbox("Enabled", &m_ssaoRenderer->m_enabled);
				m_ssaoRenderer->OnImGui();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSR")) {
				ImGui::Checkbox("Enabled", &m_ssrRenderer->m_enabled);
				m_ssrRenderer->OnImGui();
				ImGui::TreePop();
			}
		}

		if (ImGui::CollapsingHeader("Camera")) {
			if (ImGui::Button("Freecam")) {
				Camera::active = m_freeCam;
				m_selectedCamera = 0;
				glfwSetInputMode(GetApp()->GetWindow()->GetHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			if (ImGui::Button("First person")) {
				Camera::active = m_firstPersonCamera;
				m_selectedCamera = 1;
			}
			Camera::active->OnImGui();
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
void HDRPipeline::OnResize(uint width, uint height) {
	m_width = width;
	m_height = height;
	Camera::uiActive->SetViewport(0, 0, width, height);
	if (!m_initialized) return;
	Camera::active->SetViewport(0, 0, width, height);
}
