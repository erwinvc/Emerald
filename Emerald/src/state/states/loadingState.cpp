#include "stdafx.h"

void LoadingState::Initialize() {

	GetTextureManager()->Initialize();

	m_logo = GetAssetManager()->ForceLoad<Texture>(NEW(TextureLoader("Logo", "res/logo.png", false), TextureParameters(RGBA, RGBA, NEAREST, REPEAT)));
	GetTileMaterialManager()->Initialize();

	GetShaderManager()->Create("UI", "res/shader/UI");
	GetUIRenderer()->Initialize();

	m_batch = GetAssetManager()->CreateBatch<BasicAssetBatch>("Main Assets");

	m_batch->Add(NEW(CustomLoader("ImGui", [] {GetImGuiManager()->Initialize(GetApp()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Mouse", [] {GetMouse()->Initialize(GetApp()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Keyboard", [] {GetKeyboard()->Initialize(GetApp()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Thread Pool", [] {GetThreadPool()->Initialize(3); })));
	m_batch->Add(NEW(CustomLoader("Material Manager", [] {GetMaterialManager()->Initialize(); })));

	m_batch->Add(NEW(ModelLoader("Lamp", "res/lamp.obj")));
	m_batch->Add(NEW(ModelLoader("Sphere", "res/sphere.obj")));
	m_batch->Add(NEW(ModelLoader("Cube", "res/cube.obj")));

	m_batch->Add(NEW(ModelLoader("Turtle", "res/turtle.fbx")));
	m_batch->Add(NEW(ModelLoader("Mori", "res/LTEO.obj")));
	m_batch->Add(NEW(ShaderLoader("Line", "res/shader/line")));
	m_batch->Add(NEW(ShaderLoader("Geometry", "res/shader/geometry")));
	m_batch->Add(NEW(ShaderLoader("Tile", "res/shader/tile")));
	m_batch->Add(NEW(ShaderLoader("TileOld", "res/shader/tileOld")));
	m_batch->Add(NEW(ShaderLoader("Directional", "res/shader/directional")));
	m_batch->Add(NEW(ShaderLoader("Pointlight", "res/shader/pointlight")));
	m_batch->Add(NEW(ShaderLoader("HDR", "res/shader/hdr")));
	m_batch->Add(NEW(ShaderLoader("SSAO", "res/shader/ssao")));
	m_batch->Add(NEW(ShaderLoader("SSAOBlur", "res/shader/ssaoBlur")));
	m_batch->Add(NEW(ShaderLoader("Gaussian", "res/shader/gaussian")));
	m_batch->Add(NEW(ShaderLoader("EmissionAmbient", "res/shader/emissionAmbient")));
	//m_batch->Add(NEW(ShaderLoader("Gaussian",		"src/shader/gaussian")));

	m_batch->Add(NEW(ModelLoader("Tile", "res/tiles/tile.obj")));

	//for (int i = 0; i < 16; i++) {
	//	m_batch->Add(NEW(ModelLoader(Format("Tile[%d]", i), Format("res/tiles/new/%d.obj", i))));
	//}

	m_batch->Add(NEW(TextureLoader("Iridescence", "res/iridescence.png", true)));
	m_batch->Add(NEW(TextureLoader("Noise", "res/noise.png", true)));
	m_batch->Add(NEW(TextureLoader("White", "res/white.png", true)));
	//m_batch->Add(NEW(TextureLoader("BricksNormal", "res/bricksNormal.png")));
	//m_batch->Add(NEW(TextureLoader("Concrete", "res/tiles/concrete_albedo.png")));
	//m_batch->Add(NEW(TextureLoader("ConcreteSpec", "res/tiles/concrete_specular.png")));
	//m_batch->Add(NEW(TextureLoader("SphereAlbedo", "res/sphere_albedo.png")));
	m_batch->Add(NEW(TextureLoader("SphereNormal", "res/sphere_normal.png", true)));
	//m_batch->Add(NEW(TextureLoader("SphereSpec", "res/sphere_specular.png")));
	//m_batch->Add(NEW(TextureLoader("SphereEmission", "res/sphere_emission.png")));
	//m_batch->Add(NEW(TileTextureLoader("Concrete", "concrete")));
	m_batch->Add(NEW(TileTextureLoader("Cross", "cross")));
	//m_batch->Add(NEW(TextureLoader("RustAlbedo", "res/rust/rust_albedo.png")));
	//m_batch->Add(NEW(TextureLoader("RustHeight", "res/rust/rust_height.png")));
	//m_batch->Add(NEW(TextureLoader("RustMetallic", "res/rust/rust_metallic.png")));
	//m_batch->Add(NEW(TextureLoader("RustRoughness", "res/rust/rust_roughness.png")));
	//m_batch->Add(NEW(TextureLoader("RustNormal", "res/rust/rust_normal.png")));
	//m_batch->Add(NEW(ModelLoader("HP", "cathedral/sibenik.obj")));

	//m_batch->Add(NEW(CustomLoader("Tile Renderer", [] {GetTileRenderer()->Initialize(); })));
	m_batch->Add(NEW(CustomLoader("Pointlight Renderer", [] {GetPointlightRenderer()->Initialize(MeshGenerator::Sphere(10, 10), PointlightRenderer::MAX_LIGHTS); })));
	m_batch->Add(NEW(CustomLoader("Rendering Pipeline", [] {GetPipeline()->Initialize(); })));
	//m_batch->Add(NEW(CustomLoader("Tile manager", [] {GetTileManager()->Initialize(); })));
	m_batch->Add(NEW(CustomLoader("Asset Watcher", [] {GetAssetWatcher()->Initialize(); })));

	m_batch->PushLayer();
	for (State* state : GetStateManager()->GetStates()) {
		if (state != this) {
			m_batch->Add(NEW(StateLoader(state)));
		}
	}
}

void LoadingState::Update(const TimeStep& time) {
	if (m_batch->IsFinished() && GetAssetManager()->GetProgress() >= GetAssetManager()->GetProgress() - 0.01f) {
		GetTileMaterialManager()->GenerateMipmaps();
		GetStateManager()->SetState(GameStates::EDITOR);
		GetStateManager()->RemoveState(this);
	}
}
void LoadingState::RenderGeometry() {}

void LoadingState::RenderUI() {
	float progress = GetAssetManager()->GetProgress();
	Color color = Color::Mix(Color(0xFF0029), Color(0x007AFF), progress);
	GetUIRenderer()->Rect(Origin::CENTER, GetApp()->GetWidth() / 2, GetApp()->GetHeight() / 2, GetApp()->GetWidth(), GetApp()->GetHeight(), Color::Black());
	GetUIRenderer()->Rect(Origin::LEFT, Vector2(0.248f * GetApp()->GetWidth(), GetApp()->GetHeight() / 2), Vector2(0.502f * GetApp()->GetWidth(), 0.022f * GetApp()->GetHeight()), Color::White());
	GetUIRenderer()->Rect(Origin::LEFT, Vector2(0.25f * GetApp()->GetWidth(), GetApp()->GetHeight() / 2), Vector2(progress * 0.5f * GetApp()->GetWidth(), 0.02f  * GetApp()->GetHeight()), color);

	GetUIRenderer()->RenderTexture(m_logo, Origin::CENTER, GetApp()->GetWidth() / 2, GetApp()->GetHeight() * 0.85f, 0.4095f * GetApp()->GetWidth(), 0.0945f * GetApp()->GetWidth());
}
void LoadingState::OnStateImGUI() {
}
void LoadingState::OnImGUI() {
}

void LoadingState::Cleanup() {}

void LoadingState::OnEnterState() {
	GetAssetManager()->SubmitBatch(m_batch);
}
void LoadingState::OnExitState() {}
void LoadingState::OnResize(int width, int height) {}