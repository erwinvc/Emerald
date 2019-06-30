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
Texture* texIri;
Texture* texNoise;

Texture* tex;
RenderingPipeline::RenderingPipeline(int maxLights, int lightQuality) : m_hdrBuffer(FrameBuffer::Create("HDR", 1920, 1080)) {
struct col {
	byte r;
	byte g;
	byte b;
	byte a;
};

col datta[200][200] = {};


	tex = new Texture(200, 200, TextureParameters(RGBA, NEAREST, REPEAT, T_UNSIGNED_BYTE));

	for (int x = 0; x < 200; x++) {
		for (int y = 0; y < 200; y++) {
			datta[y][x] = { (byte)Math::RandomInt(0, 255), 0, 0, 255 };
		}
	}
	tex->SetData((byte*)datta);
	//Deferred
	//#Dirty get window size from config?
	m_gBuffer = NEW(GBuffer(FrameBuffer::Create("GBuffer", 1920, 1080), 1920, 1080));

	//#Dirty add proper shader asset loading
	m_geometryShader = NEW(Shader("Geo", "src/shader/geoVert.glsl", "src/shader/geoFrag.glsl"));
	m_directionalLightShader = NEW(Shader("Directional", "src/shader/directionalVert.glsl", "src/shader/directionalFrag.glsl"));
	m_pointLightShader = NEW(Shader("Pointlight", "src/shader/pointlightVert.glsl", "src/shader/pointlightFrag.glsl"));

	//HDR
	m_hdrShader = NEW(Shader("HDR", "src/shader/hdr.vert", "src/shader/hdr.frag"));
	m_hdrTexture = NEW(Texture(1920, 1080, TextureParameters(RGBA32, LINEAR, REPEAT, T_FLOAT)));
	m_hdrBuffer.AddColorBuffer(m_hdrTexture);

	//SSAO
	m_ssaoRenderer = NEW(SSAORenderer(1920, 1080));

	//UI
	m_uiShader = NEW(UIShader());
	m_uiShader->Initialize();

	float aspect = (float)(1920) / 1080;
	m_camera = NEW(FreeCam(70, aspect, 0.01f, 1000.0f));
	//m_projectionMatrix = Matrix4::Perspective(70, aspect, 0.01f, 1000.0f);

	//Shader variables
	m_geometryShader->Bind();

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

	m_pointlightRenderer = NEW(PointlightRenderer(MeshGenerator::Sphere(lightQuality, lightQuality), maxLights));

	m_quad = MeshGenerator::Quad();

	uishader = NEW(UIShader());
	uishader->Initialize();

	//int val = 10;
	//for (int x = 0; x < val; x++) {
	//	for (int y = 0; y < 1; y++) {
	//		for (int z = 0; z < val; z++) {
	//			Light light;
	//			light.sinval1 = 0;
	//			light.sinval2 = 0;
	//			light.sinval3 = 0;
	//			light.sinincrement1 = Math::RandomFloat(0, 0.1f);
	//			light.sinincrement2 = Math::RandomFloat(0, 0.1f);
	//			light.sinincrement3 = Math::RandomFloat(0, 0.1f);
	//			light.m_pos = Vector3(x * 10, 1, z * 10);
	//			light.m_original = light.m_pos;
	//			light.m_col = Color::RandomPrimary();
	//			light.m_radius = 200;
	//			lights.push_back(light);
	//			m_pointlights.push_back(Pointlight(light.m_pos, Math::RandomFloat(10, 50), light.m_col));
	//		}
	//	}
	//}

	//m_pointlights.push_back(Pointlight(m_camera->m_position, 50, Color::White()));
	model = Model();
	model.LoadModel("sponza/a.fbx");

	m_tileRenderer = NEW(TileRenderer());

	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 10; y++) {
			tiles.emplace_back(Tile(FULL), Vector2(x, y));
		}
	}

	texIri = NEW(Texture("res/irridescence.png"));
	texNoise = NEW(Texture("res/noise.png"));
}

float a1 = 10;
float a2 = 10;

void RenderingPipeline::Update(const TimeStep& time) {
	m_camera->Update(time);
	if (ButtonJustDown(VK_MOUSE_MIDDLE)) {
		m_pointlights.push_back(Pointlight(m_camera->m_position, 10, Color::RandomPrimary()));
	}

	//m_lerpAmount = Math::Clamp(m_lerpAmount + time.GetSeconds(), 0.0f, 1.0f);
	//m_projectionMatrix = Matrix4::Lerp(m_projectionMatrix, m_perspective ? m_perspectiveMatrix : m_orthoMatrix, m_lerpAmount);
}

Vector2 origin = Vector2(0, 0);
float rot = 0;
Vector2 position = Vector2(0, 0);
Vector2 sizee = Vector2(1, 1);

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
	m_geometryShader->Set("texture_iridescence", 4);
	texIri->Bind(4);
	m_geometryShader->Set("texture_noise", 5);
	texNoise->Bind(5);
	m_geometryShader->Set("scale1", m_tileRenderer->scale1);
	m_geometryShader->Set("scale2", m_tileRenderer->scale2);
	m_geometryShader->Set("scale3", m_tileRenderer->scale3);
	m_geometryShader->Set("projectionMatrix", m_camera->GetProjectionMatrix());
	m_geometryShader->Set("viewMatrix", m_camera->GetViewMatrix());

	//model.Draw(m_geometryShader);


	m_tileRenderer->Begin();
	m_gBuffer->BindTextures();

	GetWorld()->Draw(m_tileRenderer);

	//for (auto a : tiles) {
	//	m_tileRenderer->Submit(a.tile, a.position);
	//}
	//m_tileRenderer->Submit(Tile(), Vector2I(0, 0));

	m_tileRenderer->Draw();
	//RenderGeometry();

	GetLineRenderer()->Begin();

	Vector3 cast = m_rayCast.Get(m_camera);
	Vector3& pos = m_rayCast.GetGroundPosition();
	GetLineRenderer()->DrawRect(Rect((int)pos.x + 0.5f, (int)pos.z + 0.5f, 1.0f, 1.0f));
	GetLineRenderer()->Submit(m_camera->m_position, pos);
	//if (GetMouse()->ButtonJustDown(VK_MOUSE_LEFT)) {
	//	Tile* t = GetWorld()->GetTile(((int)pos.x + 0.5f), ((int)pos.z + 0.5f));
	//	if (t)
	//		t->m_type = EMPTY;
	//}
	GetLineRenderer()->DrawRect(GetWorld()->GetBoundaries());

	GetLineRenderer()->End();
	GetLineRenderer()->Draw();

	m_gBuffer->Unbind();

	GL(glDisable(GL_DEPTH_TEST));
	GL(glFrontFace(GL_CW));

	m_ssaoRenderer->Render(m_gBuffer);

	GL(glFrontFace(GL_CCW));

	//Draw to HDR
	m_hdrBuffer.Bind();
	m_hdrBuffer.Clear();

	//m_directionalLightShader->Reload();
	//Draw directional light
	m_directionalLightShader->Bind();
	m_directionalLightShader->Set("_GMisc", 0);
	m_directionalLightShader->Set("_GAlbedo", 1);
	m_directionalLightShader->Set("_GNormal", 2);
	m_directionalLightShader->Set("_GPosition", 3);
	m_directionalLightShader->Set("_SSAO", 4);
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
	m_pointLightShader->Set("_GMisc", 0);
	m_pointLightShader->Set("_GAlbedo", 1);
	m_pointLightShader->Set("_GNormal", 2);
	m_pointLightShader->Set("_GPosition", 3);
	m_pointLightShader->Set("_SSAO", 4);

	m_pointLightShader->Set("projectionMatrix", m_camera->GetProjectionMatrix());
	m_pointLightShader->Set("viewMatrix", m_camera->GetViewMatrix());
	m_pointLightShader->Set("uCameraPos", m_camera->m_position);
	m_pointLightShader->Set("shineDamper", a1);
	m_pointLightShader->Set("reflectivity", a2);
	m_pointLightShader->Set("_Diffuse", m_directionalLight.m_diffuse);
	m_pointLightShader->Set("_Specular", m_directionalLight.m_specular);
	m_pointlightRenderer->Draw(m_pointlights);

	m_hdrBuffer.Unbind();

	//Draw to screen
	GL(glDisable(GL_BLEND));

	//hdrShader->Reload();

	static int selectedTexture = 0;
	static float gamma = 1;
	static float exposure = 1;
	static int selectedTonemapping = 8;
	//m_hdrShader->Reload();
	m_hdrShader->Bind();
	m_hdrShader->Set("_HDRBuffer", 0);
	m_hdrShader->Set("_ApplyPostProcessing", m_applyPostProcessing);
	m_hdrShader->Set("_Gamma", gamma);
	m_hdrShader->Set("_Exposure", exposure);
	m_hdrShader->Set("_Tonemapping", selectedTonemapping);

	switch (selectedTexture) {
	case 0: m_hdrTexture->Bind(); break;
	case 1: m_gBuffer->m_miscTexture->Bind(); break;
	case 2: m_gBuffer->m_colorTexture->Bind(); break;
	case 3: m_gBuffer->m_normalTexture->Bind(); break;
	case 4: m_gBuffer->m_positionTexture->Bind(); break;
	case 5: m_ssaoRenderer->GetTexture()->Bind(); break;
	case 6: m_ssaoRenderer->GetRawTexture()->Bind(); break;
	}
	m_quad->Draw();
	m_hdrShader->Unbind();

	float aspect = (float)(1920) / 1080;
	m_uiShader->RenderTexture(tex->GetHandle(), origin, rot, position, sizee, false, false);

	String_t tonemapping[] = { "Linear", "SimpleReinhard", "LumaBasedReinhard", "WhitePreservingLumaBasedReinhard", "RomBinDaHouse", "Filmic", "Uncharted2", "GTA", "Aces", "Toon", "AcesFitted", "Standard" };
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
				ImGui::SliderFloat("Exposure", &exposure, 0, 5);
				ImGui::Combo("Tonemapping", &selectedTonemapping, tonemapping, NUMOF(tonemapping));
				ImGui::TreePop();
				ImGui::Separator();
			}
			if (ImGui::TreeNode("FramebufferManager")) {

			}
			if (ImGui::TreeNode("Framebuffers")) {

				if (ImGui::ImageButton(m_hdrTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 0;
				ImGui::Tooltip("Final");
				ImGui::SameLine();
				if (ImGui::ImageButton(m_gBuffer->m_miscTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 1;
				ImGui::Tooltip("Misc");
				if (ImGui::ImageButton(m_gBuffer->m_colorTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 2;
				ImGui::Tooltip("Color");
				ImGui::SameLine();
				if (ImGui::ImageButton(m_gBuffer->m_normalTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 3;
				ImGui::Tooltip("Normal");
				if (ImGui::ImageButton(m_gBuffer->m_positionTexture->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 4;
				ImGui::Tooltip("Position");

				if (ImGui::TreeNode("SSAO")) {
					if (ImGui::ImageButton(m_ssaoRenderer->GetTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 5;
					ImGui::Tooltip("SSAO blurred");
					ImGui::SameLine();
					if (ImGui::ImageButton(m_ssaoRenderer->GetRawTexture()->GetHandle(), ImVec2(192, 108), ImVec2(0, 1), ImVec2(1, 0), 2)) selectedTexture = 6;
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

		ImGui::SliderFloat("Normal", &m_tileRenderer->material->m_normalStrength, 0, 10);
		ImGui::SliderFloat("shineDamper", &a1, 0, 64);

		ImGui::SliderFloat("scale1", &m_tileRenderer->scale1, 0, 5);
		ImGui::SliderFloat("scale2", &m_tileRenderer->scale2, 0, 5);
		ImGui::SliderFloat("scale3", &m_tileRenderer->scale3, 0, 1);

		ImGui::SliderFloat2("origin", (float*)&origin, -1, 1);
		ImGui::SliderFloat("rot", (float*)&rot, 0, Math::PI);
		ImGui::SliderFloat2("position", (float*)&position, 0, 1);
		ImGui::SliderFloat2("size", (float*)&sizee, 0, 1);

		ImGui::End();
	}
}



void RenderingPipeline::RenderGeometry() {
	//model.Draw(m_geometryShader);
}

//#TODO properly resize fbos
void RenderingPipeline::Resize(uint width, uint height) {
	if (m_gBuffer) {
		DELETE(m_gBuffer);
		m_gBuffer = NEW(GBuffer(FrameBuffer::Create("GBuffer", width, height), width, height));
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
