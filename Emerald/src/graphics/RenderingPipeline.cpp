#include "stdafx.h"
#include <random>

void RenderingPipeline::Initialize(uint width, uint height) {
	m_width = width;
	m_height = height;

	//Deferred
	m_gBuffer = NEW(GBuffer(m_width, m_height));

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

	//m_gaussianShader = GetShaderManager()->Get("Gaussian");
	//m_gaussianShader->Bind();
	//m_gaussianShader->Set("_Bright", 0);

	//HDR
	m_hdrShader = GetShaderManager()->Get("HDR");
	m_hdrBuffer = GetFrameBufferManager()->Create("HDR", m_width, m_height);
	m_hdrTexture = m_hdrBuffer->AddColorBuffer("HDR", TextureParameters(RGBA16, RGBA, LINEAR, CLAMP_TO_EDGE, T_FLOAT));
	GetFrameBufferManager()->SetSelectedTexture(m_hdrTexture);
	//m_hdrBrightTexture = m_hdrBuffer->AddColorBuffer(TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT));

	//Bloom
	//m_pingPongFBO[0] = GetFrameBufferManager()->Create("PingPong1", 1920, 1080);
	//m_pingPongFBO[1] = GetFrameBufferManager()->Create("PingPong2", 1920, 1080);
	//m_pingPongTexture[0] = NEW(Texture(1920, 1080, TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	//m_pingPongTexture[1] = NEW(Texture(1920, 1080, TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	//m_pingPongFBO[0]->AddColorBuffer(m_pingPongTexture[0]);
	//m_pingPongFBO[1]->AddColorBuffer(m_pingPongTexture[1]);

	//SSAO
	m_ssaoRenderer = NEW(SSAORenderer(m_width, m_height));

	m_camera = NEW(FreeCam(70, 0.01f, 1000.0f));

	m_camera->m_position = Vector3(10, 5, -2);
	m_camera->m_rotation = Vector3(0.5, Math::PI, 0);
	//m_projectionMatrix = Matrix4::Perspective(70, aspect, 0.01f, 1000.0f);

	//Shader variables
	//m_geometryShader->Bind();

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

	if (m_wireFrame) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
}

float shineDamper = 5.0f;
float reflectivity = 0;
void RenderingPipeline::PostGeometryRender() {
	if (m_wireFrame) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));

	if (m_ssaoEnabled) m_ssaoRenderer->Render(m_gBuffer);

	//Draw to HDR
	m_hdrBuffer->Bind();
	m_hdrBuffer->Clear();

	//Draw directional light
	m_directionalLightShader->Bind();

	m_gBuffer->BindTextures();
	m_ssaoRenderer->GetTexture()->Bind(4);

	m_directionalLightShader->Set("_Color", m_directionalLight.m_color);
	m_directionalLightShader->Set("_Directional", m_directionalLight.m_direction);
	m_directionalLightShader->Set("_CameraPosition", m_camera->m_position);
	m_directionalLightShader->Set("_Diffuse", m_directionalLight.m_diffuse);
	m_directionalLightShader->Set("_Specular", m_directionalLight.m_specular);
	m_directionalLightShader->Set("_Ambient", m_directionalLight.m_ambient);
	m_directionalLightShader->Set("_SSAOEnabled", m_ssaoEnabled);

	m_quad->Bind();
	m_quad->Draw();

	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_ONE, GL_ONE));
	GL(glFrontFace(GL_CW));

	//Draw pointlights
	m_pointLightShader->Bind();
	m_gBuffer->BindTextures();

	m_pointLightShader->Set("projectionMatrix", m_camera->GetProjectionMatrix());
	m_pointLightShader->Set("viewMatrix", m_camera->GetViewMatrix());
	m_pointLightShader->Set("uCameraPos", m_camera->m_position);
	m_pointLightShader->Set("shineDamper", shineDamper);
	m_pointLightShader->Set("reflectivity", reflectivity);
	m_pointLightShader->Set("_Diffuse", m_directionalLight.m_diffuse);
	m_pointLightShader->Set("_Specular", m_directionalLight.m_specular);
	m_pointLightShader->Set("_SSAOEnabled", m_ssaoEnabled);

	GetPointlightRenderer()->End();
	GetPointlightRenderer()->Draw();

	m_hdrBuffer->Unbind();

	//Draw to screen
	GL(glDisable(GL_BLEND));

	//bool horizontal = true, first_iteration = true;
	//int amount = 10;
	//m_gaussianShader->Bind();
	//m_pingPongFBO[0]->Bind();
	//m_pingPongFBO[0]->Clear();
	//m_pingPongFBO[1]->Bind();
	//m_pingPongFBO[1]->Clear();

	//0 = fbo1;
	//1 = fbo2;

	//for (int i = 0; i < amount; i++) {
	//	m_pingPongFBO[horizontal]->Bind();
	//
	//	m_gaussianShader->Set("horizontal", horizontal);
	//
	//	if (first_iteration) m_hdrBrightTexture->Bind();
	//	else m_pingPongTexture[!horizontal]->Bind();
	//	m_quad->Draw();
	//	horizontal = !horizontal;
	//	if (first_iteration)
	//		first_iteration = false;
	//}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_hdrShader->Bind();
	m_hdrShader->Set("_HDRBuffer", 0);
	//m_hdrShader->Set("_HDRBloom", 1);
	m_hdrShader->Set("_ApplyPostProcessing", m_applyPostProcessing);
	m_hdrShader->Set("_FXAA", m_FXAA);
	m_hdrShader->Set("_Gamma", m_gamma);
	m_hdrShader->Set("_Exposure", m_exposure);
	m_hdrShader->Set("_Tonemapping", m_selectedTonemapping);
	m_hdrShader->Set("_ScreenSize", (float)GetApplication()->GetWidth(), (float)GetApplication()->GetHeight());

	//m_hdrShader->Set("_Bloom", m_bloom);
	GetFrameBufferManager()->GetSelectedTexture()->Bind();

	//m_pingPongTexture[1]->Bind(1);
	m_quad->Bind();
	m_quad->Draw();
	m_hdrShader->Unbind();
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
			m_camera->OnImGui();
		}

		if (ImGui::CollapsingHeader("Memory")) {
			GetMemory()->OnImGui();
		}

		if (ImGui::Checkbox("Wireframe", &m_wireFrame)) GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL)); 
		ImGui::SliderFloat("Shinedamper", &shineDamper, 0.001f, 10);
		ImGui::SliderFloat("Reflectivity", &reflectivity, 0, 5);
		ImGui::EndTabItem();
	}
	//ImGui::Checkbox("Bloom", &m_bloom);


	//ImGui::SliderFloat("shineDamper", &a1, 0, 64);
}


//#TODO properly resize fbos
void RenderingPipeline::OnResize(uint width, uint height) {
	if (!m_initialized) return;
	m_width = width;
	m_height = height;
	m_gBuffer->Resize(width, height);
	m_hdrBuffer->Resize(width, height);
	m_ssaoRenderer->Resize(width, height);
	m_camera->UpdateProjectionMatrix();
}
