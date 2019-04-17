#include "stdafx.h"
#include <random>

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

TileType GetRandomType() {
	switch (Math::RandomInt(0, 5)) {
	case 0: return FULL;
	case 1: return INNER;
	case 2: return OUTER;
	case 3: return SLOPE;
	case 4: return VALLEY;
	}
	return FULL;
}

struct t {
	Tile tile;
	Vector2 position;

	t(Tile& t, Vector2& p) : tile(t), position(p) {}
};
vector<t> tiles;
vector<Vector3> ssaoKernel;

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
	m_hdrShader = new Shader("HDR", "src/shader/hdr.vert", "src/shader/hdr.frag");
	m_hdrTexture = new Texture(1920, 1080, TextureParameters(RGBA, LINEAR, REPEAT, T_FLOAT));
	m_hdrBuffer = new FrameBuffer("HDR", 1920, 1080, Color(0.0f, 0.0f, 0.3f, 1.0f));
	m_hdrBuffer->AddColorBuffer(m_hdrTexture);

	//SSAO
	m_ssaoShader = new Shader("SSAO", "src/shader/ssaoVert.glsl", "src/shader/ssaoFrag.glsl");
	m_ssaoTexture = new Texture(1920, 1080, TextureParameters(RGBA, LINEAR, REPEAT, T_FLOAT));
	m_ssaoBuffer = new FrameBuffer("SSAO", 1920, 1080, Color(0.3f, 0.0f, 0.0f, 1.0f));
	m_ssaoBuffer->AddColorBuffer(m_ssaoTexture);
	//for (unsigned int i = 0; i < 64; ++i) {
	//    Vector3 sample(Math::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0, Math::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0, Math::RandomFloat(0.0f, 1.0f));
	//    sample = sample.Normalize();
	//    sample *= Math::RandomFloat(0.0f, 1.0f);
	//    float scale = float(i) / 64.0;
	//    scale = Math::Lerp(0.1f, 1.0f, scale * scale);
	//    sample *= scale;
	//    ssaoKernel.push_back(sample);
	//}

	std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
	std::default_random_engine generator;
	for (unsigned int i = 0; i < 64; ++i) {
		Vector3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample = sample.Normalize();
		sample *= randomFloats(generator);
		float scale = float(i) / 64.0;

		// scale samples s.t. they're more aligned to center of kernel
		scale = Math::Lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	//SSAO noise
	vector<Color> ssaoNoise;
	for (unsigned int i = 0; i < 16; i++) {
		ssaoNoise.push_back(Color(Math::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0, Math::RandomFloat(0.0f, 1.0f) * 2.0 - 1.0, 0.0f, 1));
	}
	m_ssaoNoiseTexture = new Texture(4, 4, (Byte*)ssaoNoise.data(), TextureParameters(RGBA32, NEAREST, REPEAT, T_FLOAT));

	//UI
	m_uiShader = new UIShader();
	m_uiShader->Initialize();

	m_projectionMatrix = Matrix4::Perspective(70, (float)(1920) / 1080, 0.1f, 3000.0f);

	//Shader variables
	m_geometryShader->Bind();
	m_geometryShader->Set("projectionMatrix", m_projectionMatrix);

	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("uColorTex", 0);
	m_directionalLightShader->Set("uNormalTex", 1);
	m_directionalLightShader->Set("uPositionTex", 2);
	m_directionalLightShader->Set("ssao", 3);

	m_pointLightShader->Bind();
	m_pointLightShader->Set("uColorTex", 0);
	m_pointLightShader->Set("uNormalTex", 1);
	m_pointLightShader->Set("uPositionTex", 2);
	m_pointLightShader->Set("ssao", 3);
	m_pointLightShader->Set("projectionMatrix", m_projectionMatrix);

	m_pointlightRenderer = new PointlightRenderer(MeshGenerator::Sphere(lightQuality, lightQuality), maxLights);

	m_quad = MeshGenerator::Quad();

	model.LoadModel("sponza/sponza.obj");

	uishader = new UIShader();
	uishader->Initialize();

	//int val = 16;
	//for (int x = -val; x < val; x++) {
	//    for (int y = -val; y < val; y++) {
	//        for (int z = -val; z < val; z++) {
	//            Light light;
	//            light.sinval1 = 0;
	//            light.sinval2 = 0;
	//            light.sinval3 = 0;
	//            light.sinincrement1 = Math::RandomFloat(0, 0.1f);
	//            light.sinincrement2 = Math::RandomFloat(0, 0.1f);
	//            light.sinincrement3 = Math::RandomFloat(0, 0.1f);
	//            light.m_pos = Vector3(x * 100, y * 50 + 50, z * 50);
	//            light.m_original = light.m_pos;
	//            light.m_col = Color::RandomPrimary();
	//            light.m_radius = 200;
	//            lights.push_back(light);
	//            m_pointlights.push_back(Pointlight(light.m_pos, Math::RandomFloat(10, 50), light.m_col));
	//        }
	//    }
	//}

   // m_pointlights.push_back(Pointlight(m_camera->m_position, 50, Color::White()));

	m_tileRenderer = new TileRenderer();

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			tiles.emplace_back(Tile(FULL), Vector2(x, y));
		}
	}
}


float a1 = 10;
float a2 = 10;

float radius = 0.5;
float bias = 0.025;
int power = 1;
void RenderingPipeline::Update(const TimeStep& time) {
	if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
		m_pointlights.push_back(Pointlight(m_camera->m_position, 10, Color::RandomPrimary()));
	}
}
void RenderingPipeline::Render() {
	//m_pointlights[0].m_position = m_camera->m_position;
	//for (Light& l : lights) {
	//    l.sinval1 += l.sinincrement1;
	//    l.sinval2 += l.sinincrement2;
	//    l.sinval3 += l.sinincrement3;
	//    l.m_pos.x += Math::Sin(l.sinval1);
	//    l.m_pos.y += Math::Sin(l.sinval2);
	//    l.m_pos.z += Math::Sin(l.sinval3);
	//}
	//
	//static float sin = 0.00;
	//sin += 0.002f;
	//for (int i = 0; i < m_pointlights.size(); i++) {
	//    Pointlight& light = m_pointlights[i];
	//    Light& l = lights[i];
	//    l.sinval1 += l.sinincrement1;
	//    l.sinval2 += l.sinincrement2;
	//    l.sinval3 += l.sinincrement3;
	//
	//    light.m_position.x += Math::Sin(l.sinval1);
	//    light.m_position.y += Math::Sin(l.sinval2);
	//    light.m_position.z += Math::Sin(l.sinval3);
	//}

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

	m_tileRenderer->Begin(m_camera, m_projectionMatrix);
	m_gBuffer->BindTextures();


	for (auto a : tiles) {
		m_tileRenderer->Submit(a.tile, a.position);
	}
	//m_tileRenderer->Submit(Tile(), Vector2I(0, 0));

	m_tileRenderer->Draw();
	//RenderGeometry();

	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));
	GL(glFrontFace(GL_CW));

	//SSAO
	m_gBuffer->BindTextures();
	m_ssaoBuffer->Bind();
	m_ssaoBuffer->Clear();
	m_ssaoShader->Bind();
	m_ssaoShader->Set("gPosition", 0);
	m_ssaoShader->Set("gNormal", 1);
	m_ssaoShader->Set("texNoise", 2);
	m_ssaoShader->Set("radius", radius);
	m_ssaoShader->Set("bias", bias);
	m_ssaoShader->Set("power", power);

	// Send kernel + rotation 
	for (unsigned int i = 0; i < 64; ++i)
		m_ssaoShader->Set(("samples[" + std::to_string(i) + "]").c_str(), ssaoKernel[i]);
	m_ssaoShader->Set("projection", m_projectionMatrix);
	m_ssaoShader->Set("view", m_camera->GetViewMatrix());
	m_gBuffer->BindTextures();
	m_gBuffer->m_positionTexture->Bind(0);
	m_gBuffer->m_normalTexture->Bind(1);
	m_ssaoNoiseTexture->Bind(2);
	m_quad->Draw();
	m_ssaoBuffer->Unbind();

	GL(glFrontFace(GL_CCW));

	//Draw to HDR
	m_hdrBuffer->Bind();
	m_hdrBuffer->Clear();

	//m_directionalLightShader->Reload();
	//Draw directional light
	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("_GMisc", 0);
	m_directionalLightShader->Set("_GAlbedo", 1);
	m_directionalLightShader->Set("_GNormal", 2);
	m_directionalLightShader->Set("_GPosition", 3);
	m_directionalLightShader->Set("_SSAO", 4);
	m_gBuffer->BindTextures();
	m_ssaoTexture->Bind(4);

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
	m_pointLightShader->Set("_GMisc", 0);
	m_pointLightShader->Set("_GAlbedo", 1);
	m_pointLightShader->Set("_GNormal", 2);
	m_pointLightShader->Set("_GPosition", 3);
	m_pointLightShader->Set("_SSAO", 4);

	m_pointLightShader->Set("viewMatrix", m_camera->GetViewMatrix());
	m_pointLightShader->Set("uCameraPos", m_camera->m_position);
	m_pointLightShader->Set("shineDamper", a1);
	m_pointLightShader->Set("reflectivity", a2);
	m_pointLightShader->Set("_Diffuse", m_directionalLight.m_diffuse);
	m_pointLightShader->Set("_Specular", m_directionalLight.m_specular);
	m_pointlightRenderer->Draw(m_pointlights);

	m_hdrBuffer->Unbind();


	//Draw to screen
	GL(glDisable(GL_BLEND));

	//hdrShader->Reload();

	static int selectedTexture = 0;
	static float gamma = 1;
	static int selectedTonemapping = 8;
	m_hdrShader->Bind();
	m_hdrShader->Set("applyPostProcessing", m_applyPostProcessing);
	m_hdrShader->Set("gamma", gamma);
	m_hdrShader->Set("tonemapping", selectedTonemapping);

	switch (selectedTexture) {
	case 0: m_hdrTexture->Bind(); break;
	case 1: m_gBuffer->m_miscTexture->Bind(); break;
	case 2: m_gBuffer->m_colorTexture->Bind(); break;
	case 3: m_gBuffer->m_normalTexture->Bind(); break;
	case 4: m_gBuffer->m_positionTexture->Bind(); break;
	case 5: m_ssaoTexture->Bind(); break;
	}
	m_quad->Draw();

	String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted" };
	//ImGui::ShowTestWindow();

	//uishader->RenderTexture(m_hdrBuffer->GetTexture()->GetHandle(), Vector2(0, 0), 0, Vector2(0, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_colorTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.25f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_normalTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.5f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	//uishader->RenderTexture(m_gBuffer->m_positionTexture->GetHandle(), Vector2(0, 0), 0, Vector2(0.75f, 0), Vector2(0.25f, 0.25f), Vector2(1920, 1080), false, false);
	static bool open = true;
	//static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	//opt_flags |= ImGuiDockNodeFlags_PassthruDockspace;
	//ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	//
	//ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(viewport->Pos);
	//ImGui::SetNextWindowSize(viewport->Size);
	//ImGui::SetNextWindowViewport(viewport->ID);
	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
	//window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	//
	//// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
   ////if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)

	//ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	//opt_flags |= ImGuiDockNodeFlags_PassthruDockspace;
	//ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	//ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	//
	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
	//ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(viewport->Pos);
	//ImGui::SetNextWindowSize(viewport->Size);
	//ImGui::SetNextWindowViewport(viewport->ID);
	//window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	//
	//if (ImGui::Begin("DockWindow", &open, window_flags)) {
	//
	//   // 
	//   // static bool done = false;
	//   // if (!done) {
	//   //     
	//   //     ImGuiDockNode* node = ImGui::DockBuilderGetCentralNode(dockspace_id);
	//   //     ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.40f, NULL, &dockspace_id);
	//   //     done = true;
	//   // }
	//    //node->
	//    //LOG("%d", node->ID);
	//    //ImGui::DockBuilderDockWindow("Emerald", node->ID);
	//
	//    if (ImGui::BeginMenuBar()) {
	//        if (ImGui::BeginMenu("Scene")) {
	//            if (ImGui::MenuItem("Flag: NoSplit", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 opt_flags ^= ImGuiDockNodeFlags_NoSplit;
	//            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
	//            if (ImGui::MenuItem("Flag: NoResize", "", (opt_flags & ImGuiDockNodeFlags_NoResize) != 0))                opt_flags ^= ImGuiDockNodeFlags_NoResize;
	//            if (ImGui::MenuItem("Flag: PassthruDockspace", "", (opt_flags & ImGuiDockNodeFlags_PassthruDockspace) != 0))       opt_flags ^= ImGuiDockNodeFlags_PassthruDockspace;
	//            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
	//            ImGui::Separator();
	//            ImGui::EndMenu();
	//        }
	//
	//        ImGui::EndMenuBar();
	//    }
	//
	//    ImGui::End();
	//}

	static bool op3en;
	if (ImGui::Begin("Emerald###Window", &op3en, ImVec2(576, 680), -1)) {
		ImGui::DragFloat3("Directional", (float*)&m_directionalLight, 0.01f);
		if (ImGui::CollapsingHeader("HDR")) {
			ImGui::Checkbox("Post processing", &m_applyPostProcessing);
			if (ImGui::TreeNode("Tonemapping")) {
				ImGui::SliderFloat("Gamma", &gamma, 0, 5);
				ImGui::Combo("Tonemapping", &selectedTonemapping, tonemapping, NUMOF(tonemapping));
				ImGui::TreePop();
				ImGui::Separator();
			}

			if (ImGui::TreeNode("Framebuffers")) {
				if (ImGui::ImageButton((void*)m_hdrTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 0;
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)m_gBuffer->m_miscTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 1;
				if (ImGui::ImageButton((void*)m_gBuffer->m_colorTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 2;
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)m_gBuffer->m_normalTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 3;
				if (ImGui::ImageButton((void*)m_gBuffer->m_positionTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 4;
				ImGui::SameLine();
				if (ImGui::ImageButton((void*)m_ssaoTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 5;
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
		}

		ImGui::SliderFloat("Normal", &m_tileRenderer->material->m_normalStrength, 0, 10);
		ImGui::SliderFloat("shineDamper", &a1, 0, 64);
		ImGui::SliderFloat("bias", &bias, 0, 0.25f);
		ImGui::SliderFloat("radius", &radius, 0, 25);
		ImGui::SliderInt("power", &power, 0, 64);

		ImGui::End();
		//auto a = ImGui::GetCurrentWindow();
	}

	//static bool p_open = true;
	//static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
	//
	//// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	//// because it would be confusing to have two docking targets within each others.
	//ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	//ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(viewport->Pos);
	//ImGui::SetNextWindowSize(viewport->Size);
	//ImGui::SetNextWindowViewport(viewport->ID);
	//window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	//
	//// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	//if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
	//    window_flags |= ImGuiWindowFlags_NoBackground;
	//
	//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	//ImGui::Begin("DockSpace Demo", &p_open, window_flags);
	//ImGui::PopStyleVar();
	//
	//// Dockspace
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
	//    ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
	//    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
	//}
	//
	//if (ImGui::BeginMenuBar()) {
	//    if (ImGui::BeginMenu("Docking")) {
	//        // Disabling fullscreen would allow the window to be moved to the front of other windows, 
	//        // which we can't undo at the moment without finer window depth/z control.
	//        //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);
	//
	//        if (ImGui::MenuItem("Flag: NoSplit", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 opt_flags ^= ImGuiDockNodeFlags_NoSplit;
	//        if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
	//        if (ImGui::MenuItem("Flag: NoResize", "", (opt_flags & ImGuiDockNodeFlags_NoResize) != 0))                opt_flags ^= ImGuiDockNodeFlags_NoResize;
	//        if (ImGui::MenuItem("Flag: PassthruDockspace", "", (opt_flags & ImGuiDockNodeFlags_PassthruDockspace) != 0))       opt_flags ^= ImGuiDockNodeFlags_PassthruDockspace;
	//        if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
	//        ImGui::Separator();
	//        if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
	//            p_open = false;
	//        ImGui::EndMenu();
	//    }
	//
	//    ImGui::EndMenuBar();
	//}
	//
	//ImGui::End();

}



void RenderingPipeline::RenderGeometry() {
	model.Draw(m_geometryShader);
}

//#TODO properly resize fbos
void RenderingPipeline::Resize(uint width, uint height) {
	if (m_gBuffer) {
		delete m_gBuffer;
		m_gBuffer = new GBuffer(width, height);
	}
	if (m_hdrBuffer) {
		delete m_hdrBuffer;
		delete m_hdrTexture;
		m_hdrTexture = new Texture(width, height, TextureParameters(RGBA, LINEAR, REPEAT, T_FLOAT));
		m_hdrBuffer = new FrameBuffer("HDR", width, height);
		m_hdrBuffer->AddColorBuffer(m_hdrTexture);
	}
	if (m_ssaoBuffer) {
		delete m_ssaoBuffer;
		delete m_ssaoTexture;
		m_ssaoTexture = new Texture(width, height, TextureParameters(RGBA, LINEAR, REPEAT, T_FLOAT));
		m_ssaoBuffer = new FrameBuffer("SSAO", width, height);
		m_ssaoBuffer->AddColorBuffer(m_ssaoTexture);
	}
}
