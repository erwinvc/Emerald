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

	m_hdrFBO = GetFrameBufferManager()->Create("HDR", FBOScale::FULL);
	m_hdrTexture = m_hdrFBO->AddBuffer("HDR", TextureParameters(INT_RGB16, DATA_RGBA, NEAREST, CLAMP_TO_EDGE, T_FLOAT));
	GetFrameBufferManager()->SetSelectedTexture(m_hdrTexture);

	m_ssaoRenderer = NEW(SSAORenderer());
	m_ssrRenderer = NEW(SSRRenderer());
	m_lineRenderer = NEW(LineRenderer());
	m_bloomRenderer = NEW(BloomRenderer());

	m_freeCam = NEW(FreeCam(glm::vec2(1920, 1080), 70, 0.5f, 500.0f));
	m_firstPersonCamera = NEW(FirstPersonCam(glm::vec2(1920, 1080), 70, 0.5f, 500.0f));

	Camera::active = m_freeCam;

	m_firstPersonCamera->transform.m_position = glm::vec3(14, 0, -2);
	m_firstPersonCamera->transform.m_rotation = glm::vec3(0, Math::PI, 0);
	Camera::active->transform.m_position = glm::vec3(0.0f, 20.0f, 0.0f);
	Camera::active->transform.m_rotation = glm::vec3(0.0f, Math::PI, 0.0f);

	//Final
	m_finalFBO = GetFrameBufferManager()->Create("Final", FBOScale::FULL);
	m_finalTexture = m_finalFBO->AddBuffer("Final", TextureParameters(INT_RGB, DATA_RGB, LINEAR, CLAMP_TO_EDGE, T_UNSIGNED_BYTE));

	m_quad = MeshGenerator::Quad();

	m_initialized = true;
}

void HDRPipeline::Render() {
	if (!m_initialized) {
		m_spriteRenderer->Begin();
		GetStateManager()->RenderGeometry(this);
		GetFrameBufferManager()->BindDefaultFBO();
		m_spriteRenderer->End();
		GLUtils::EnableBlending();
		m_spriteRenderer->Draw();
		GLUtils::DisableBlending();

		return;
	}

	m_spriteRenderer->Begin();
	m_lineRenderer->Begin();

	PreGeometryRender();
	GetStateManager()->RenderGeometry(this);
	PostGeometryRender();

	m_spriteRenderer->End();
	GLUtils::EnableBlending();
	m_spriteRenderer->Draw();
	GLUtils::DisableBlending();

	m_finalFBO->Blit(nullptr);
	GetFrameBufferManager()->BindDefaultFBO();


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
	GLUtils::EnableDepthTest();
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
	GL(glEnable(GL_STENCIL_TEST));
	GL(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	
	m_ubo->data._CameraPosition = Camera::active->transform.m_position;
	m_ubo->data._Projection = Camera::active->GetProjectionMatrix();
	m_ubo->data._View = Camera::active->GetViewMatrix();
	m_ubo->data._InverseProjection = Camera::active->GetInverseProjectionMatrix();
	m_ubo->data._InverseView = Camera::active->GetInverseViewMatrix();
	m_ubo->data._SSAOEnabled = m_ssaoRenderer->m_enabled;
	m_ubo->data._CameraPlanes = glm::vec2(Camera::active->GetNear(), Camera::active->GetFar());
	m_ubo->data._ViewPort = glm::vec2(Camera::active->GetViewport().z, Camera::active->GetViewport().w);
	m_ubo->SetData();

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

	GLUtils::DisableDepthTest();
	m_ssaoRenderer->Render(m_gBuffer);

	//Blit depth to HDR FBO
	m_hdrFBO->Bind();
	m_hdrFBO->Clear();
	m_gBuffer->GetFBO()->BlitDepthOnly(m_hdrFBO);
	m_hdrFBO->Bind();

	//Draw to HDR
	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_ONE, GL_ONE));

	//Emission
	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(2);
	m_emissionAmbientShader->Bind();
	m_emissionAmbientShader->Set("_AmbientIntensity", m_ambientIntensity);
	m_quad->Bind();
	m_quad->Draw();

	//Draw directional light
	m_directionalLightShader->Bind();

	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(4);
	m_directionalLightShader->Set("_Color", m_directionalLight.GetColor());
	m_directionalLightShader->Set("_Directional", m_directionalLight.GetDirection());
	m_quad->Bind();
	m_quad->Draw();

	GL(glFrontFace(GL_CW));

	//Draw pointlights
	m_pointLightShader->Bind();
	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(4);
	GetPointlightRenderer()->End();
	GetPointlightRenderer()->Draw();

	m_hdrFBO->Unbind();

	//Draw to finalFBO
	GL(glDisable(GL_BLEND));

	//SSR
	m_ssrRenderer->Draw(this);

	//Bloom
	m_bloomRenderer->Draw(this);

	//Blit depth to final FBO
	m_finalFBO->Bind();
	m_finalFBO->Clear();
	m_hdrFBO->BlitDepthOnly(m_finalFBO);
	m_finalFBO->Bind();

	m_hdrShader->Bind();
	m_hdrShader->Set("_Bloom", m_bloomRenderer->m_enabled);
	m_hdrShader->Set("_BloomMultiplier", m_bloomRenderer->m_bloomMultiplier);
	m_hdrShader->Set("_ApplyPostProcessing", GetFrameBufferManager()->GetSelectedTexture() == m_hdrTexture && m_applyPostProcessing);
	m_hdrShader->Set("_FXAA", m_FXAA);
	m_hdrShader->Set("_Gamma", m_gamma);
	m_hdrShader->Set("_Exposure", m_exposure);
	m_hdrShader->Set("_Tonemapping", m_selectedTonemapping);
	m_hdrShader->Set("_ScreenSize", glm::vec2(m_width, m_height));
	//m_hdrShader->Set("_Chromatic", 0);

	GetFrameBufferManager()->GetSelectedTexture()->Bind(0);
	m_bloomRenderer->GetBloomedTexture()->Bind(1);
	m_quad->Bind();
	m_quad->Draw();

	//Draw lines
	m_lineRenderer->End();
	m_lineRenderer->Draw();
}

void HDRPipeline::OnImGUI() {
	if (ImGui::BeginTabItem("Pipeline")) {
		const String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted", "Standard", "Cherno" };
		if (ImGui::CollapsingHeader("Post Processing")) {
			UI::Begin();
			UI::Bool("Post processing", &m_applyPostProcessing);
			UI::Bool("FXAA", &m_FXAA);
			UI::Bool("Bloom", &m_bloomRenderer->m_enabled);
			UI::Bool("SSAO", &m_ssaoRenderer->m_enabled);
			UI::Bool("SSR", &m_ssrRenderer->m_enabled);
			UI::Separator();
			UI::Float("Gamma", &m_gamma, 0, 5);
			UI::Float("Exposure", &m_exposure, 0, 5);
			UI::Combo("Tonemapping", &m_selectedTonemapping, tonemapping, NUMOF(tonemapping));
			UI::End();
			
			if (ImGui::TreeNode("Bloom")) {
				UI::Begin();
				m_bloomRenderer->OnImGui();
				UI::End();
				ImGui::TreePop();
			}
			if (ImGui::TreeNode("SSAO")) {
				UI::Begin();
				m_ssaoRenderer->OnImGui();
				UI::End();
				ImGui::TreePop();
			}
			UI::Dummy();
			//if (ImGui::TreeNode("SSR")) {
			//	UI::Begin();
			//	m_ssrRenderer->OnImGui();
			//	UI::End();
			//	ImGui::TreePop();
			//}
			//if (ImGui::TreeNode("Tonemapping")) {
			//	UI::Begin();
			//
			//	UI::End();
			//	ImGui::TreePop();
			//	ImGui::Separator();
			//}
		}
		if (ImGui::CollapsingHeader("Scene")) {
			if (ImGui::TreeNode("Lighting")) {
				UI::Begin();
				UI::Float("Ambient", &m_ambientIntensity, 0.0f, 1.0f);
				m_directionalLight.OnImGui();
				UI::End();
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
