#include "stdafx.h"

Model* m_mori;
Model* m_block;
Entity* m_moriEntity;
Entity* m_blockEntity;
Material* m_moriMaterial;
Material* m_blockMaterial;
Model* m_reflectionModel;
Shader* m_geometryShader;
Shader* m_directionalShadowShader;

Texture* m_albedo;
Texture* m_normal;
Texture* m_metallic;
Texture* m_roughness;
Texture* m_emission;

Camera* m_oldCam;
Camera* m_editorCam;

TextureLoader albedoLoader("EditorAlbedo", "res/editor/editor_albedo.psd", false, TextureParameters(INT_SRGB, DATA_UNK, NEAREST), true);
TextureLoader normalLoader("EditorNormal", "res/editor/editor_normal.psd", false, TextureParameters(INT_RGB, DATA_UNK, LINEAR), true);
TextureLoader roughnessLoader("EditorRoughness", "res/editor/editor_roughness.psd", false, TextureParameters(INT_RED, DATA_UNK, NEAREST), true);
TextureLoader metallicLoader("EditorMetallic", "res/editor/editor_metallic.psd", false, TextureParameters(INT_RED, DATA_UNK, NEAREST), true);
TextureLoader emissionLoader("EditorEmission", "res/editor/editor_emission.psd", false, TextureParameters(INT_RGB, DATA_UNK, NEAREST), true);

void EditorState::Initialize() {
	m_directionalShadowShader = GetShaderManager()->Get("DirectionalShadow");
	m_reflectionModel = GetAssetManager()->Get<Model>("EditorReflection");
	m_mori = GetAssetManager()->Get<Model>("Mori");
	m_block = GetAssetManager()->Get<Model>("Cube");
	m_geometryShader = GetShaderManager()->Get("Geometry");
	m_moriEntity = NEW(Entity(NEW(Model(m_mori->GetMeshes()[0]->Copy()))));
	m_blockEntity = NEW(Entity(NEW(Model(m_block->GetMeshes()[0]->Copy()))));
	m_moriEntity->transform.position.x = -1.1f;
	m_blockEntity->transform.position.x = 1.1f;
	m_moriEntity->transform.position.y = 0.5f;
	m_blockEntity->transform.position.y = 0.5f;
	m_blockEntity->transform.size = glm::vec3(0.5f, 0.5f, 0.5f);
	m_albedo = GetTextureManager()->GetWhiteTexture();
	m_normal = GetTextureManager()->GetNormalTexture();
	m_metallic = GetTextureManager()->GetBlackTexture();
	m_roughness = GetTextureManager()->GetWhiteTexture();
	m_emission = GetTextureManager()->GetBlackTexture();
	m_moriMaterial = GetMaterialManager()->Create("EditorMori", m_geometryShader);
	m_moriMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Albedo", &m_albedo, 0)));
	m_moriMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Normal", &m_normal, 1)));
	m_moriMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Roughness", &m_roughness, 2)));
	m_moriMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Metallic", &m_metallic, 3)));
	m_moriMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Emission", &m_emission, 4)));
	m_blockMaterial = GetMaterialManager()->Create("EditorBlock", m_geometryShader);
	m_blockMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Albedo", &m_albedo, 0)));
	m_blockMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Normal", &m_normal, 1)));
	m_blockMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Roughness", &m_roughness, 2)));
	m_blockMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Metallic", &m_metallic, 3)));
	m_blockMaterial->AddOnBindCallback(NEW(MaterialCallbackTexturePtr("_Emission", &m_emission, 4)));
	m_moriEntity->m_model->SetMaterial(m_moriMaterial);
	m_blockEntity->m_model->SetMaterial(m_blockMaterial);

	m_editorCam = NEW(FreeCam(glm::vec2(1920, 1080), 70, 0.05f, 500.0f));
	m_editorCam->transform.position = glm::vec3(0.2f, 1.7f, 2.5f);
	m_editorCam->transform.rotation = glm::vec3(0.5f, 0.0f, 0.0f);


	m_albedo = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&albedoLoader);
	m_normal = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&normalLoader);
	m_roughness = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&roughnessLoader);
	m_metallic = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&metallicLoader);
	m_emission = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&emissionLoader);

	GetAssetWatcher()->AddDirectory("res/editor", false);
	GetAssetWatcher()->AddChangeHandler(WatchType::CHANGED, "psd", [&](const Tracker& file) {
		ReloadTexture(file.m_path.GetFileName());
	});
}

void EditorState::RenderGeometry(HDRPipeline* pipeline) {
	m_moriEntity->Draw();
	m_blockEntity->Draw();
	m_reflectionModel->Draw();
}

void EditorState::ReloadTexture(const String& file) {
	if (Utils::EndsWith(file, "albedo")) m_albedo = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&albedoLoader);
	if (Utils::EndsWith(file, "normal")) m_normal = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&normalLoader);
	if (Utils::EndsWith(file, "roughness")) m_roughness = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&roughnessLoader);
	if (Utils::EndsWith(file, "metallic")) m_metallic = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&metallicLoader);
	if (Utils::EndsWith(file, "emission")) m_emission = GetAssetManager()->ForceLoad<Texture>((AssetLoader*)&emissionLoader);
}
void EditorState::RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) {
	m_moriEntity->DrawShadow(m_directionalShadowShader);
	m_blockEntity->DrawShadow(m_directionalShadowShader);
	m_reflectionModel->DrawShadow(m_directionalShadowShader);
}

void EditorState::Update(const TimeStep& time) {
	Camera::active->Update();

	m_moriEntity->transform.rotation.y += Math::QUARTER_PI / 60.0f;
	m_blockEntity->transform.rotation.y += Math::QUARTER_PI / 60.0f;
}

ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoDockingInCentralNode;
void EditorState::OnImGuiViewport() {
	ImGui::SetNextWindowDockID(m_dockspaceCenter, ImGuiCond_Always);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport##1", nullptr, window_flags);

	ImVec2 viewportSize = ImGui::GetContentRegionAvail();
	GetClient()->pipeline->OnResize((uint)viewportSize.x, (uint)viewportSize.y);
	GetFrameBufferManager()->OnResize((uint)viewportSize.x, (uint)viewportSize.y);
	ImVec2 viewportOffset = ImGui::GetCursorPos();
	Camera::active->SetViewport((uint)viewportOffset.x, (uint)viewportOffset.y, (uint)viewportSize.x, (uint)viewportSize.y);
	ImGui::Image((void*)(uint64)GetClient()->pipeline->GetFinalTexture()->GetHandle(), viewportSize, { 0, 1 }, { 1, 0 });

	static int counter = 0;
	auto windowSize = ImGui::GetWindowSize();
	ImVec2 minBound = ImGui::GetWindowPos();
	minBound.x += viewportOffset.x;
	minBound.y += viewportOffset.y;

	ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
	if ((ImGui::IsMouseHoveringRect(minBound, maxBound) || ImGui::IsWindowFocused()) && ImGui::GetFrontMostPopupModal() == nullptr) {
		GetMouse()->OverrideImGuiCapture();
		GetKeyboard()->OverrideImGuiCapture();
	}

	ImGui::End();
	ImGui::PopStyleVar(3);

	ImGui::SetNextWindowDockID(m_dockspaceLeft, ImGuiCond_Always);
	ImGui::Begin("Editor", nullptr, window_flags);

	if (ImGui::Button("Save texture")) ImGui::OpenPopup("Save Textures");

	if (ImGui::BeginPopupModal("Save Textures", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Enter a file name    ");
		ImGui::Dummy(ImVec2(0, 20));
		static char nameBuffer[16];
		ImGui::PushItemWidth(ImGui::GetWindowWidth());
		ImGui::InputText("", nameBuffer, 16, ImGuiInputTextFlags_CallbackCharFilter, ImGuiManager::PathFilter);
		ImGui::PopItemWidth();

		bool disabled = strlen(nameBuffer) == 0;
		ImGui::BeginDisable(disabled);
		if (ImGui::Button("Save")) {
			m_albedo->SaveAsPNG(Format_t("res/editor/%s_albedo.png", nameBuffer));
			m_normal->SaveAsPNG(Format_t("res/editor/%s_normal.png", nameBuffer));
			m_metallic->SaveAsPNG(Format_t("res/editor/%s_metallic.png", nameBuffer));
			m_roughness->SaveAsPNG(Format_t("res/editor/%s_roughness.png", nameBuffer));
			m_emission->SaveAsPNG(Format_t("res/editor/%s_emission.png", nameBuffer));
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndDisable(disabled);
		ImGui::SameLine(ImGui::GetWindowWidth() - 60);
		if (ImGui::Button("Cancel")) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//UI::Begin();
	//UI::End();
	ImGui::End();

	ImGui::SetNextWindowDockID(m_dockspaceBottom, ImGuiCond_Always);
	Logger::OnImGui();


}

void EditorState::OnImGUI() {

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dock", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	// Dockspace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		dockspace_id = ImGui::GetID("MyDockspace");

		if (!ImGui::DockBuilderGetNode(dockspace_id)) {
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_AutoHideTabBar);

			m_dockspaceCenter = dockspace_id;
			m_dockspaceRight = ImGui::DockBuilderSplitNode(m_dockspaceCenter, ImGuiDir_Right, 0.2f, nullptr, &m_dockspaceCenter);
			m_dockspaceLeft = ImGui::DockBuilderSplitNode(m_dockspaceCenter, ImGuiDir_Left, 0.2f, nullptr, &m_dockspaceCenter);
			m_dockspaceBottom = ImGui::DockBuilderSplitNode(m_dockspaceCenter, ImGuiDir_Down, 0.2f, nullptr, &m_dockspaceCenter);

			ImGui::DockBuilderFinish(m_dockspaceCenter);
		}
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		OnImGuiViewport();

		ImGui::End();
	}
}

void EditorState::OnEnterState() {
	m_oldCam = Camera::active;
	Camera::active = m_editorCam;
}

void EditorState::OnExitState() {
	Camera::active = m_oldCam;
	Camera::active->SetViewport(0, 0, GetClient()->GetWidth(), GetClient()->GetHeight());
	GetClient()->pipeline->OnResize(GetClient()->GetWidth(), GetClient()->GetHeight());
	GetFrameBufferManager()->OnResize(GetClient()->GetWidth(), GetClient()->GetHeight());
}

void EditorState::Cleanup() {
	DELETE(m_moriEntity->m_model);
	DELETE(m_blockEntity->m_model);
	DELETE(m_moriEntity);
	DELETE(m_blockEntity);
}