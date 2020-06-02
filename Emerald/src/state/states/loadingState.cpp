#include "stdafx.h"

#define MAX 256
Sprite loadingSprite;

void LoadingState::Initialize() {
	m_animatedLogo = GetAssetManager()->ForceLoad<Texture>(new TextureLoader("AnimatedLogo", "res/animatedLogo.png", false));
	loadingSprite = Sprite(m_animatedLogo, Color::White(), 8, 62, 0, 1.0f / 31.0f);
	
	GetTextureManager()->Initialize();
	GetTileMaterialManager()->Initialize();
	GetImGuiManager()->Initialize(GetClient()->GetWindow());

	GetShaderManager()->Create("Geometry", "res/shader/geometry");
	GetShaderManager()->Create("UI", "res/shader/UI");
	GetShaderManager()->Create("Line", "res/shader/line");

	m_batch = GetAssetManager()->CreateBatch<BasicAssetBatch>("Main Assets");

	m_batch->Add(NEW(CustomLoader("Mouse", [] {GetMouse()->Initialize(GetClient()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Keyboard", [] {GetKeyboard()->Initialize(GetClient()->GetWindow()); })));
	m_batch->Add(NEW(CustomLoader("Thread Pool", [] {GetThreadPool()->Initialize(3); })));
	m_batch->Add(NEW(CustomLoader("Material Manager", [] {GetMaterialManager()->Initialize(); })));

	//m_batch->Add(NEW(PBRTextureLoader("metal", "metal")));
	//m_batch->Add(NEW(ModelLoader("Sponza", "res/sponza2/sponzaPBR.obj")));
	m_batch->Add(NEW(ModelLoader("Mori", "res/LTEO.obj")));
	m_batch->Add(NEW(ModelLoader("Cube", "res/cube.obj")));

	m_batch->Add(NEW(ShaderLoader("DirectionalShadowChunk", "res/shader/directionalShadowChunk")));
	m_batch->Add(NEW(ShaderLoader("DirectionalShadow", "res/shader/directionalShadow")));
	m_batch->Add(NEW(ShaderLoader("DepthCubemap", "res/shader/depthCubemap", true)));
	m_batch->Add(NEW(ShaderLoader("Tile", "res/shader/tile")));
	m_batch->Add(NEW(ShaderLoader("Chunk", "res/shader/chunk")));
	m_batch->Add(NEW(ShaderLoader("TileOld", "res/shader/tileOld")));
	m_batch->Add(NEW(ShaderLoader("Directional", "res/shader/directional")));
	m_batch->Add(NEW(ShaderLoader("Pointlight", "res/shader/pointlight")));
	m_batch->Add(NEW(ShaderLoader("HDR", "res/shader/hdr")));
	m_batch->Add(NEW(ShaderLoader("HBAO", "res/shader/hbao")));
	m_batch->Add(NEW(ShaderLoader("SSAO", "res/shader/ssao")));
	m_batch->Add(NEW(ShaderLoader("SSAOBlur", "res/shader/ssaoBlur")));
	m_batch->Add(NEW(ShaderLoader("Bloom", "res/shader/bloom")));
	m_batch->Add(NEW(ShaderLoader("Gaussian", "res/shader/gaussian")));
	m_batch->Add(NEW(ShaderLoader("EmissionAmbient", "res/shader/emissionAmbient")));
	m_batch->Add(NEW(ShaderLoader("SSR", "res/shader/ssr")));
	m_batch->Add(NEW(ShaderLoader("Simple", "res/shader/simple")));
	m_batch->Add(NEW(ShaderLoader("Atmosphere", "res/shader/atmosphere")));

	m_batch->Add(NEW(BlockTextureArrayLoader("Dirt", "dirt")));
	m_batch->Add(NEW(BlockTextureArrayLoader("DirtGlow", "dirtGlow")));
	m_batch->Add(NEW(BlockTextureArrayLoader("Metal", "metal")));

	m_batch->Add(NEW(TextureLoader("Crosshair", "res/crosshair.png", false, TextureParameters(INT_RGBA, DATA_UNK, NEAREST))));
	m_batch->Add(NEW(TextureLoader("Dirt", "res/dirt_albedo.png", true, TextureParameters(INT_SRGB, DATA_UNK, NEAREST))));
	m_batch->Add(NEW(TextureLoader("DirtNormal", "res/dirt_normal.png", true, TextureParameters(INT_RGB, DATA_UNK, LINEAR))));
	m_batch->Add(NEW(TextureLoader("DirtMetallic", "res/dirt_metallic.png", true, TextureParameters(INT_RED, DATA_UNK, NEAREST))));
	m_batch->Add(NEW(TextureLoader("DirtRoughness", "res/dirt_roughness.png", true, TextureParameters(INT_RED, DATA_UNK, NEAREST))));

	m_batch->Add(NEW(CustomLoader("Pointlight Renderer", [] {GetPointlightRenderer()->Initialize(MeshGenerator::Sphere(10, 10), PointlightRenderer::MAX_LIGHTS); })));
	m_batch->Add(NEW(CustomLoader("Rendering Pipeline", [] {GetClient()->pipeline->Initialize(); })));
	m_batch->Add(NEW(CustomLoader("Asset Watcher", [] {GetAssetWatcher()->Initialize(); })));

	m_batch->PushLayer();
	for (State* state : GetStateManager()->GetStates()) {
		if (state != this) {
			m_batch->Add(NEW(StateLoader(state)));
		}
	}
}

void LoadingState::RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) {}

void LoadingState::Update(const TimeStep& time) {
	if (m_batch->IsFinished() && GetAssetManager()->GetProgress() >= GetAssetManager()->GetProgress() - 0.01f) {
		GetTileMaterialManager()->GenerateMipmaps();
		GetStateManager()->SetState(GameStates::MENU);
		GetStateManager()->RemoveState(this);
	}
}
void LoadingState::RenderGeometry(HDRPipeline* pipeline) {
	pipeline->UIRect(Origin::CENTER, pipeline->Width() / 2.0f, pipeline->Height() / 2.0f, pipeline->Width(), pipeline->Height(), 0, Color::Black());
	loadingSprite.Draw(pipeline, Origin::CENTER, pipeline->Width() - 100, 100, 100, 100, 0);
}

void LoadingState::OnStateImGUI() {}
void LoadingState::OnImGUI() {}
void LoadingState::Cleanup() {}

void LoadingState::OnEnterState() {
	GetAssetManager()->SubmitBatch(m_batch);
}
void LoadingState::OnExitState() {}
void LoadingState::OnResize(int width, int height) {}