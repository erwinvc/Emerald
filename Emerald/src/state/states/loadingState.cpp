#include "stdafx.h"

void LoadingState::Initialize() {

	GetTextureManager()->Initialize();
	GetTileTextureManager()->Initialize();

	m_logo = GetAssetManager()->ForceLoad<Texture>(NEW(TextureLoader("Logo", "res/Emerald_logo_no_background.png")));

	GetShaderManager()->Create("UI", "src/shader/UI");
	GetUIRenderer()->Initialize();

	m_batch = GetAssetManager()->CreateBatch("Main Assets");

	m_batch->Add(NEW(CustomLoader("ImGui", [] {GetImGuiManager()->Initialize(GetApplication()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Mouse", [] {GetMouse()->Initialize(GetApplication()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Keyboard", [] {GetKeyboard()->Initialize(GetApplication()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Material Manager", [] {GetMaterialManager()->Initialize(); })));

	m_batch->Add(NEW(ModelLoader("dragon", "sponza/a.fbx")));
	m_batch->Add(NEW(ShaderLoader("Line", "src/shader/line")));
	m_batch->Add(NEW(ShaderLoader("Geometry", "src/shader/geometry")));
	m_batch->Add(NEW(ShaderLoader("Tile", "src/shader/tile")));
	m_batch->Add(NEW(ShaderLoader("Directional", "src/shader/directional")));
	m_batch->Add(NEW(ShaderLoader("Pointlight", "src/shader/pointlight")));
	m_batch->Add(NEW(ShaderLoader("HDR", "src/shader/hdr")));
	m_batch->Add(NEW(ShaderLoader("SSAO", "src/shader/ssao")));
	m_batch->Add(NEW(ShaderLoader("SSAOBlur", "src/shader/ssaoBlur")));
	m_batch->Add(NEW(TileTextureLoader("white", "test")));
	//m_batch->Add(NEW(TileTextureLoader("obsidian", "obsidian")));
	//m_batch->Add(NEW(ShaderLoader("Gaussian",		"src/shader/gaussian")));

	for (int i = 0; i < 16; i++) {
		m_batch->Add(NEW(ModelLoader(Format("Tile[%d]", i), Format("tiles/new/%d.obj", i))));
	}

	m_batch->Add(NEW(TextureLoader("Irridescence", "res/irridescence.png")));
	m_batch->Add(NEW(TextureLoader("Noise", "res/noise.png")));
	m_batch->Add(NEW(TextureLoader("White", "res/white.png")));
	m_batch->Add(NEW(TextureLoader("BricksNormal", "sponza/bricksNormal.png")));
	m_batch->Add(NEW(ModelLoader("HP", "cathedral/sibenik.obj")));

	m_batch->Add(NEW(CustomLoader("Tile renderer", [] {GetTileRenderer()->Initialize(); })));
	m_batch->Add(NEW(CustomLoader("Pointlight renderer", [] {GetPointlightRenderer()->Initialize(MeshGenerator::Sphere(10, 10), PointlightRenderer::MAX_LIGHTS); })));
	m_batch->Add(NEW(CustomLoader("Rendering Pipeline", [] {GetPipeline()->Initialize(GetApplication()->GetWidth(), GetApplication()->GetHeight()); })));

	//m_batch->Add(NEW(CustomLoader("Tile manager", [] {GetTileManager()->Initialize(); })));
	for (State* state : GetStateManager()->GetStates()) {
		if (state != this) {
			m_batch->Add(NEW(StateLoader(state)));
		}
	}
}

Vector2 origin = Vector2(0, 0);
float rot = 0;
Vector2 position = Vector2(0, 0);
Vector2 sizee = Vector2(1, 1);

float progress = 0;
Color color;

void LoadingState::Update(const TimeStep& time) {
	progress = Math::Ease(progress, GetAssetManager()->GetProgress(), 6);
	color = Color::Mix(Color(0xde9c96), Color(0x96deae), progress);
	if (m_batch->IsFinished()) {
		GetTileTextureManager()->GenerateMipmaps();
		GetStateManager()->SetState(GameStates::GAME);
		GetStateManager()->RemoveState(this);
	}
}
void LoadingState::RenderGeometry() {}

void LoadingState::RenderUI() {
	GetUIRenderer()->Rect(Origin::CENTER, Vector2(0, 0), Vector2(1, 1));
	GetUIRenderer()->Rect(Origin::LEFT, Vector2(0.498f, 0.0f), Vector2(0.502f, 0.022f), Color::Black());
	GetUIRenderer()->Rect(Origin::LEFT, Vector2(0.5f, 0.0f), Vector2(progress* 0.5f, 0.02f), color);

	//GetUIRenderer()->RenderTexture(m_logo, Origin::CENTER, position, sizee);
}
void LoadingState::OnImGUI() {
	ImGui::SliderFloat("Progress", &progress, 0, 1);

	ImGui::SliderFloat2("origin", (float*)&origin, -1, 1);
	ImGui::SliderFloat("rot", (float*)&rot, 0, Math::PI);
	ImGui::SliderFloat2("position", (float*)&position, 0, 1);
	ImGui::SliderFloat2("size", (float*)&sizee, 0, 1);
}

void LoadingState::Cleanup() {}

void LoadingState::OnEnterState() {
	GetAssetManager()->SubmitBatch(m_batch);
}
void LoadingState::OnExitState() {}
void LoadingState::OnResize(int width, int height) {}