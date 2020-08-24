#include "stdafx.h"

VoxelState::~VoxelState() {
	DELETE(m_world);
}

void VoxelState::Initialize() {
	m_world = NEW(ClientWorld());
	crosshair = GetAssetManager()->Get<Texture>("Crosshair");
	m_dcm = NEW(DepthCubemap());
	m_pointlight = Pointlight(glm::vec3(0, 0, 0), 15, Color::White());
	m_directionalShadowShader = GetShaderManager()->Get("DirectionalShadowChunk");
	m_omniDirectionalShadowShader = GetShaderManager()->Get("DepthCubemap");
}

void VoxelState::Update(const TimeStep& time) {
	Camera::active->Update();
	m_world->Update(time);
	if (aaa) m_pointlight.position = Camera::active->transform.position;
	if (KeyDown('J'))m_pointlight.position.x += 0.016f;
	if (KeyDown('L'))m_pointlight.position.x -= 0.016f;
	if (KeyDown('I'))m_pointlight.position.z += 0.016f;
	if (KeyDown('K'))m_pointlight.position.z -= 0.016f;
	if (KeyDown('U'))m_pointlight.position.y += 0.016f;
	if (KeyDown('O'))m_pointlight.position.y -= 0.016f;

	float goalAmbient = Math::Clamp(Math::Map(Camera::active->transform.position.y, -8.0f, 0.0f, 0.0f, 0.05f), 0.0f, 1.0f);
	GetClient()->pipeline->ambientIntensity = Math::Lerp(GetClient()->pipeline->ambientIntensity, goalAmbient, 0.02f);
}

void VoxelState::RenderGeometry(HDRPipeline* pipeline) {
	m_world->RenderGeometry(pipeline);
	GetPointlightRenderer()->Submit(m_pointlight);
	pipeline->UIRect(Origin::CENTER, pipeline->Width() / 2.0f, pipeline->Height() / 2.0f, 35, 35, 0, Color::White(), crosshair);

	//AABB player(Camera::active->transform.position.x - 0.3f, Camera::active->transform.position.y - 1.42f, Camera::active->transform.position.z - 0.3f, Camera::active->transform.position.x + 0.3f, Camera::active->transform.position.y + 0.38f, Camera::active->transform.position.z + 0.3f);
	//player.Expand(0.5f, 0.5f, 0.5f);
	//auto& aabbs = m_world->GetAABBs(player);
	//for (auto& aabb : aabbs) {
	//	pipeline->DrawAABB(aabb, Color::Cyan(), true);
	//}
	//pipeline->DrawAABB(player, Color::Red(), true);
	//const float offset = 0.5f;
	//AABB player(Camera::active->transform.position.x - 0.3f, Camera::active->transform.position.y - 1.42f, Camera::active->transform.position.z - 0.3f, Camera::active->transform.position.x + 0.3f, Camera::active->transform.position.y + 0.38f, Camera::active->transform.position.z + 0.3f);
	//for (float x = Math::Floor(Camera::active->transform.position.x - box2.dimensions.x - offset); x < Camera::active->transform.position.x + box2.dimensions.x + offset; x++) {
	//	for (float y = Math::Floor(Camera::active->transform.position.y - box2.dimensions.y - offset); y < Camera::active->transform.position.y + 0.38f + offset; y++) {
	//		for (float z = Math::Floor(Camera::active->transform.position.z - box2.dimensions.z - offset); z < Camera::active->transform.position.z + box2.dimensions.z + offset; z++) {
	//			BlockState* blockState = nullptr;
	//			GetWorld()->GetBlock(glm::vec3(x, y, z), blockState);
	//			if (blockState && blockState->blockID != 0) {
	//				AABB aabb(x + ((float)blockState->dWest / 16.0f), y + ((float)blockState->dDown / 16.0f), z + ((float)blockState->dNorth / 16.0f), x + 1 - ((float)blockState->dEast / 16.0f), y + 1 - ((float)blockState->dUp / 16.0f), z + 1 - ((float)blockState->dSouth / 16.0f));
	//				pipeline->DrawAABB(aabb, Color::Cyan(), true);
	//			}
	//		}
	//	}
	//}
}

void VoxelState::RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) {
	if (type == ShadowType::DIRECTIONAL) {
		m_world->RenderChunks(m_directionalShadowShader);
		GetFrameBufferManager()->BindDefaultFBO();
	} else if (type == ShadowType::OMNIDIRECTIONAL) {
		m_world->RenderChunks(m_omniDirectionalShadowShader);
		GetFrameBufferManager()->BindDefaultFBO();
	}
}

void VoxelState::OnStateImGUI() {
	if (ImGui::Button("Disconnect")) {
		GetClient()->Disconnect();
		GameStates::VOXEL->GetWorld()->Clear();
		GetStateManager()->SetState(GameStates::MENU);
	}

	m_dcm->OnImGui();
	UI::Begin();

	UI::Color4("Pointlight color", &m_pointlight.color);
	UI::Float("Size", &m_pointlight.radius, 0.0f, 15.0f);
	UI::Bool("AAA", &aaa);
	UI::End();
}
void VoxelState::OnImGUI() {
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

			ImGui::DockBuilderFinish(m_dockspaceCenter);
		}
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_NoDockingInCentralNode);
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

		for (int i = 0; i < m_world->m_entities.size(); i++) {
			String_t name = m_world->m_entities[i].name;
			if (i == GetClient()->GetPlayerID()) continue;
			if (m_world->m_entities[i].active) {
				float dist = glm::distance(m_world->m_entities[i].position, Camera::active->transform.position);
				if (dist < 10.0f) {
					float mapped = Math::Map(dist, 8.0f, 10.0f, 255.0f, 0.0f);
					glm::vec2 clientPos;
					if (Coordinates::WorldToScreen(m_world->m_entities[i].position + glm::vec3(0.0f, 1.0f, 0.0f), clientPos)) {
						ImVec2 windowPos = ImGui::FindWindowByName("Viewport##1")->Pos;
						ImVec2 width = ImGui::CalcTextSize(&name[0], &name[15]);
						ImGui::GetOverlayDrawList()->AddText(ImGui::GetIO().Fonts->Fonts[0], 24, ImVec2(clientPos.x + windowPos.x - (width.x / 2.0f), windowPos.y + (GetClient()->GetHeight() - clientPos.y)),/*AnchorPoints::GetAnchor(Anchors::MIDDLECENTER, ImVec2(-660, -280)),*/IM_COL32(225, 225, 225, dist > 8.0f ? mapped : 255), name);
					}
				}
			}
		}

		static int counter = 0;
		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();
		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };
		if (ImGui::IsMouseHoveringRect(minBound, maxBound) || ImGui::IsWindowFocused()) {
			GetMouse()->OverrideImGuiCapture();
			GetKeyboard()->OverrideImGuiCapture();
		}

		ImGui::PopStyleVar(3);

		ImGui::End();

		ImGui::End();
	}
}