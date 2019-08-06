#include "stdafx.h"
#include <random>

void RenderingPipeline::Initialize(int maxLights, int lightQuality) {
	m_hdrBuffer = GetFrameBufferManager()->Create("HDR", 1920, 1080);
	//Deferred
	//#Dirty get window size from config?
	m_gBuffer = NEW(GBuffer(GetFrameBufferManager()->Create("GBuffer", 1920, 1080), 1920, 1080));

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

	//m_gaussianShader = GetShaderManager()->Get("Gaussian");
	//m_gaussianShader->Bind();
	//m_gaussianShader->Set("_Bright", 0);

	//HDR
	m_hdrShader = GetShaderManager()->Get("HDR");
	m_hdrTexture = NEW(Texture(1920, 1080, TextureParameters(RGBA32, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	m_hdrBrightTexture = NEW(Texture(1920, 1080, TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	m_hdrBuffer->AddColorBuffer(m_hdrTexture);
	m_hdrBuffer->AddColorBuffer(m_hdrBrightTexture);

	//Bloom
	//m_pingPongFBO[0] = GetFrameBufferManager()->Create("PingPong1", 1920, 1080);
	//m_pingPongFBO[1] = GetFrameBufferManager()->Create("PingPong2", 1920, 1080);
	//m_pingPongTexture[0] = NEW(Texture(1920, 1080, TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	//m_pingPongTexture[1] = NEW(Texture(1920, 1080, TextureParameters(RGBA16, LINEAR, CLAMP_TO_EDGE, T_FLOAT)));
	//m_pingPongFBO[0]->AddColorBuffer(m_pingPongTexture[0]);
	//m_pingPongFBO[1]->AddColorBuffer(m_pingPongTexture[1]);

	//SSAO
	m_ssaoRenderer = NEW(SSAORenderer(1920, 1080));

	float aspect = (float)(1920) / 1080;
	m_camera = NEW(FreeCam(70, aspect, 0.01f, 1000.0f));
	m_camera->m_position = Vector3(10, 5, -2);
	m_camera->m_rotation = Vector3(0.5, Math::PI, 0);
	//m_projectionMatrix = Matrix4::Perspective(70, aspect, 0.01f, 1000.0f);

	//Shader variables
	//m_geometryShader->Bind();

	m_pointlightRenderer = NEW(PointlightRenderer(MeshGenerator::Sphere(lightQuality, lightQuality), maxLights));

	m_quad = MeshGenerator::Quad();

	m_initialized = true;
}

//void RenderingPipeline::Update(const TimeStep& time) {
	//m_lerpAmount = Math::Clamp(m_lerpAmount + time.GetSeconds(), 0.0f, 1.0f);
	//m_projectionMatrix = Matrix4::Lerp(m_projectionMatrix, m_perspective ? m_perspectiveMatrix : m_orthoMatrix, m_lerpAmount);
//}

void RenderingPipeline::PreGeometryRender() {
	GL(glEnable(GL_DEPTH_TEST));
	GL(glDepthMask(true));
	GL(glDisable(GL_BLEND));
	GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL(glEnable(GL_CULL_FACE));
	GL(glFrontFace(GL_CCW));

	//Draw to gBuffer
	m_gBuffer->Bind();
	m_gBuffer->Clear();
}

float shineDamper = 0;
float reflectivity = 0;
void RenderingPipeline::PostGeometryRender() {
	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));
	GL(glFrontFace(GL_CW));

	m_ssaoRenderer->Render(m_gBuffer);

	GL(glFrontFace(GL_CCW));

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
	GL(glDrawArrays(GL_TRIANGLES, 0, 3));

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
	m_pointlightRenderer->Draw(m_pointlights);

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
	m_hdrShader->Set("_Gamma", m_gamma);
	m_hdrShader->Set("_Exposure", m_exposure);
	m_hdrShader->Set("_Tonemapping", m_selectedTonemapping);
	//m_hdrShader->Set("_Bloom", m_bloom);
	switch (m_selectedTexture) {
	case 0: m_hdrTexture->Bind(); break;
	case 1: m_hdrBrightTexture->Bind(); break;
	case 2: m_gBuffer->m_miscTexture->Bind(); break;
	case 3: m_gBuffer->m_colorTexture->Bind(); break;
	case 4: m_gBuffer->m_normalTexture->Bind(); break;
	case 5: m_gBuffer->m_positionTexture->Bind(); break;
	case 6: m_ssaoRenderer->GetTexture()->Bind(); break;
	case 7: m_ssaoRenderer->GetRawTexture()->Bind(); break;
	//case 8: m_pingPongTexture[0]->Bind(); break;
	//case 9: m_pingPongTexture[1]->Bind(); break;
	}

	//m_pingPongTexture[1]->Bind(1);

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
	String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted", "Standard" };
	ImGui::LabelText("", Format_t("State: %s", GetStateManager()->GetState()->GetName().c_str()));
	ImGui::DragFloat3("Directional", (float*)&m_directionalLight, 0.01f);
	if (ImGui::CollapsingHeader("HDR")) {
		ImGui::Checkbox("Post processing", &m_applyPostProcessing);
		if (ImGui::TreeNode("Tonemapping")) {
			ImGui::SliderFloat("Gamma", &m_gamma, 0, 5);
			ImGui::SliderFloat("Exposure", &m_exposure, 0, 5);
			ImGui::Combo("Tonemapping", &m_selectedTonemapping, tonemapping, NUMOF(tonemapping));

			ImGui::TreePop();
			ImGui::Separator();
		}
		if (ImGui::TreeNode("FramebufferManager")) {
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Framebuffers")) {

			if (ImGui::ImageButton(m_hdrTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 0;
			ImGui::Tooltip("Final");
			ImGui::SameLine();
			if (ImGui::ImageButton(m_hdrBrightTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 1;
			ImGui::Tooltip("Bright");

			if (ImGui::ImageButton(m_gBuffer->m_miscTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 2;
			ImGui::Tooltip("Misc");
			ImGui::SameLine();
			if (ImGui::ImageButton(m_gBuffer->m_colorTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 3;
			ImGui::Tooltip("Color");

			if (ImGui::ImageButton(m_gBuffer->m_normalTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 4;
			ImGui::Tooltip("Normal");
			ImGui::SameLine();
			if (ImGui::ImageButton(m_gBuffer->m_positionTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 5;
			ImGui::Tooltip("Position");

			//if (ImGui::ImageButton(m_pingPongTexture[0]->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 8;
			//ImGui::Tooltip("Pingpong1");
			//ImGui::SameLine();
			//if (ImGui::ImageButton(m_pingPongTexture[1]->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 9;
			//ImGui::Tooltip("Pingpong2");

			if (ImGui::TreeNode("SSAO")) {
				if (ImGui::ImageButton(m_ssaoRenderer->GetTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 6;
				ImGui::Tooltip("SSAO blurred");
				ImGui::SameLine();
				if (ImGui::ImageButton(m_ssaoRenderer->GetRawTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) m_selectedTexture = 7;
				ImGui::Tooltip("SSAO raw");
				ImGui::TreePop();
			}
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
	ImGui::SliderFloat("Shinedamper", &shineDamper, 0, 5);
	ImGui::SliderFloat("Reflectivity", &reflectivity, 0, 5);
	//ImGui::Checkbox("Bloom", &m_bloom);


	//ImGui::SliderFloat("shineDamper", &a1, 0, 64);
}


//#TODO properly resize fbos
void RenderingPipeline::OnResize(uint width, uint height) {
	if (m_gBuffer) {
		DELETE(m_gBuffer);
		m_gBuffer = NEW(GBuffer(GetFrameBufferManager()->Create("GBuffer", width, height), width, height));
	}
	//if (m_hdrBuffer) {
	//	DELETE(m_hdrBuffer);
	//	m_hdrTexture = NEW(Texture(width, height, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT)));
	//	m_hdrBuffer = FrameBuffer::Create("HDR", width, height);
	//	m_hdrBuffer->AddColorBuffer(m_hdrTexture);
	//}
	if (m_ssaoRenderer) {
		DELETE(m_ssaoRenderer);
		m_ssaoRenderer = NEW(SSAORenderer(width, height));
	}
}
