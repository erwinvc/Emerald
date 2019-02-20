#include "stdafx.h"

struct Light {
	float sinval1;
	float sinval2;
	float sinval3;
	float sinincrement1;
	float sinincrement2;
	float sinincrement3;
	Vector3 m_pos;
	Vector3 m_original;
	Color m_col;
	float m_radius;

};

static Model model;

static Light* currentLight;
static vector<Light> lights;
static UIShader* uishader;
static void NewLight() {
	Light light;
	light.m_col = Color::Random();
	light.m_pos = Vector3();
	light.m_radius = 5;
	lights.push_back(light);
	currentLight = &lights.back();
}

void RenderingPipeline::Initialize(int maxLights, int lightQuality) {
	//Deferred
	//#Dirty get window size from config?
	m_gBuffer = new GBuffer(1920, 1080);
	m_gBuffer->SetClearColor(Color(0.0f, 0.0f, 0.3f, 1.0f));

	//#Dirty add proper shader asset loading
	m_geometryShader = new Shader("Geo", "src/shader/geoVert.glsl", "src/shader/geoFrag.glsl");
	m_directionalLightShader = new Shader("Directional", "src/shader/directionalVert.glsl", "src/shader/directionalFrag.glsl");
	m_pointLightShader = new Shader("Pointlight", "src/shader/pointlightVert.glsl", "src/shader/pointlightFrag.glsl");

	//HDR
	m_hdrBuffer = new FrameBuffer(1920, 1080, false, Color(0.0f, 0.0f, 0.3f, 1.0f));
	m_hdrShader = new Shader("HDR", "src/shader/hdr.vert", "src/shader/hdr.frag");

	//UI
	m_uiShader = new UIShader();
	m_uiShader->Initialize();

	m_projectionMatrix = Matrix4::Perspective(70, (float)(1920) / 1080, 0.1f, 3000.0f);

	//Shader variables
	m_geometryShader->Bind();
	m_geometryShader->Set("projectionMatrix", m_projectionMatrix);
	m_geometryShader->Set("uDiffTex", 0);
	m_geometryShader->Set("uBumpTex", 1);

	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("uColorTex", 0);
	m_directionalLightShader->Set("uNormalTex", 1);
	m_directionalLightShader->Set("uPositionTex", 2);

	m_pointLightShader->Bind();
	m_pointLightShader->Set("uColorTex", 0);
	m_pointLightShader->Set("uNormalTex", 1);
	m_pointLightShader->Set("uPositionTex", 2);
	m_pointLightShader->Set("projectionMatrix", m_projectionMatrix);

	m_pointlightRenderer = new PointlightRenderer(MeshGenerator::Sphere(lightQuality, lightQuality), maxLights);

	m_quad = MeshGenerator::Quad();

	model.LoadModel("sponza/sponza.obj");

	uishader = new UIShader();
	uishader->Initialize();

	int val = 16;
	for (int x = -val; x < val; x++) {
		for (int y = -val; y < val; y++) {
			for (int z = -val; z < val; z++) {
				Light light;
				light.sinval1 = 0;
				light.sinval2 = 0;
				light.sinval3 = 0;
				light.sinincrement1 = Math::RandomFloat(0, 0.1f);
				light.sinincrement2 = Math::RandomFloat(0, 0.1f);
				light.sinincrement3 = Math::RandomFloat(0, 0.1f);
				light.m_pos = Vector3(x * 100, y * 50 + 50, z * 50);
				light.m_original = light.m_pos;
				light.m_col = Color::RandomPrimary();
				light.m_radius = 200;
				lights.push_back(light);
				m_pointlights.push_back(Pointlight(light.m_pos, Math::RandomFloat(10, 50), light.m_col));
			}
		}
	}
}

void RenderingPipeline::Render() {
	for (Light& l : lights) {
		l.sinval1 += l.sinincrement1;
		l.sinval2 += l.sinincrement2;
		l.sinval3 += l.sinincrement3;
		l.m_pos.x += Math::Sin(l.sinval1);
		l.m_pos.y += Math::Sin(l.sinval2);
		l.m_pos.z += Math::Sin(l.sinval3);
	}

	static float sin = 0.00;
	sin += 0.002f;
	for (int i = 0; i < m_pointlights.size(); i++) {
		Pointlight& light = m_pointlights[i];
		Light& l = lights[i];
		l.sinval1 += l.sinincrement1;
		l.sinval2 += l.sinincrement2;
		l.sinval3 += l.sinincrement3;

		light.m_position.x += Math::Sin(l.sinval1);
		light.m_position.y += Math::Sin(l.sinval2);
		light.m_position.z += Math::Sin(l.sinval3);
	}

	//GL(glEnable(GL_MULTISAMPLE));
	GL(glEnable(GL_DEPTH_TEST));
	GL(glDepthMask(true));
	GL(glDisable(GL_BLEND));
	GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
	GL(glEnable(GL_CULL_FACE));
	GL(glFrontFace(GL_CCW));

	//Draw to gBuffer
	m_gBuffer->Bind();
	m_gBuffer->Clear();

	//f
	m_geometryShader->Bind();
	m_geometryShader->Set("viewMatrix", m_camera->GetViewMatrix());

	model.Draw(m_geometryShader);
	//RenderGeometry();

	m_gBuffer->Unbind();

	//Draw to HDR
	m_hdrBuffer->Bind();
	m_hdrBuffer->Clear();

	//Draw directional light
	m_directionalLightShader->Bind();
	m_gBuffer->BindTextures();
	m_directionalLightShader->Set("uCameraPos", m_camera->m_position);
	m_directionalLightShader->Set("directional", m_directionalLight);
	GL(glDrawArrays(GL_TRIANGLES, 0, 3));

	GL(glDisable(GL_DEPTH_TEST));
	GL(glEnable(GL_BLEND));
	GL(glBlendFunc(GL_ONE, GL_ONE));
	GL(glFrontFace(GL_CW));

	//Draw pointlights
	m_pointLightShader->Bind();
	m_gBuffer->BindTextures();
	m_pointLightShader->Set("viewMatrix", m_camera->GetViewMatrix());

	m_pointlightRenderer->Draw(m_pointlights);

	m_hdrBuffer->Unbind();

	//Draw to screen
	GL(glDisable(GL_BLEND));

	//hdrShader->Reload();

	static int selectedTexture = 0;
	static float gamma = 1;
	static int selectedTonemapping = 0;
	m_hdrShader->Bind();
	m_hdrShader->Set("applyPostProcessing", m_applyPostProcessing);
	m_hdrShader->Set("gamma", gamma);
	m_hdrShader->Set("tonemapping", selectedTonemapping);
	switch (selectedTexture)
	{
	case 0: m_hdrBuffer->GetTexture()->Bind(); break;
	case 1: m_gBuffer->m_colorTexture->Bind(); break;
	case 2: m_gBuffer->m_normalTexture->Bind(); break;
	case 3: m_gBuffer->GetFBO()->GetDepthTexture()->Bind(); break;
	}
	m_quad->Draw();

	String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted" };

	//uishader->RenderTexture(m_hdrBuffer->GetTexture()->GetHandle(), Vector2(0, 0), 0, Vector2(0, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_colorTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.25f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_normalTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.5f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_positionTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.75f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	static bool open;
	ImGui::Begin("Emerald", &open, ImVec2(576, 680), -1, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar);
	if (ImGui::CollapsingHeader("HDR")) {
		if (ImGui::TreeNode("Tonemapping"))
		{
			ImGui::SliderFloat("Gamma", &gamma, 0, 5);
			ImGui::Combo("Tonemapping", &selectedTonemapping, tonemapping, NUMOF(tonemapping));
			ImGui::TreePop();
			ImGui::Separator();
		}

		if (ImGui::TreeNode("Framebuffers"))
		{
			if (ImGui::ImageButton((void*)m_hdrBuffer->GetTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2))selectedTexture = 0;
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)m_gBuffer->m_colorTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2))selectedTexture = 1;
			if (ImGui::ImageButton((void*)m_gBuffer->m_normalTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2))selectedTexture = 2;
			ImGui::SameLine();
			if (ImGui::ImageButton((void*)m_gBuffer->GetFBO()->GetDepthTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2))selectedTexture = 3;
			ImGui::TreePop();
			ImGui::Separator();
		}
	}
	//ImGui::TreePush("aa");
	//const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
	//static int listbox_item_current = 1;
	//ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
	//ImGui::TreePop();
	//ImGui::Text("Save change to the following items?");


	//ImGui::ListBox("a", &selected, )
	ImGui::Checkbox("Post processing", &m_applyPostProcessing);

	ImGui::End();
}

void RenderingPipeline::RenderGeometry() {
	model.Draw(m_geometryShader);
}

void RenderingPipeline::Resize(uint width, uint height) {
	if (m_gBuffer) {
		delete m_gBuffer;
		m_gBuffer = new GBuffer(width, height);
	}
	if (m_hdrBuffer) {
		delete m_hdrBuffer;
		m_hdrBuffer = new FrameBuffer(width, height);
	}
}
