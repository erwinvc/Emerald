#include "stdafx.h"

Model* m_mori;
Shader* m_geometryShader;
Texture* iri;
Texture* noise;

Entity* entity[64];
vector<float> pointsX;
vector<float> pointsY;
void EditorState::Initialize() {
	m_mori = GetAssetManager()->Get<Model>("Mori");
	BasicMaterial* mat = GetMaterialManager()->Create<BasicMaterial>("Turtle");
	mat->SetAlbedo(GetAssetManager()->Get<Texture>("White"));
	//mat->SetNormal(GetAssetManager()->Get<Texture>("SphereNormal"));
	m_mori->SetMaterial(mat);
	m_geometryShader = GetShaderManager()->Get("Geometry");
	iri = GetAssetManager()->Get<Texture>("Iridescence");
	noise = GetAssetManager()->Get<Texture>("Noise");

	for (int i = 0; i < 64; i++) {
		entity[i] = new Entity(m_mori);
		entity[i]->m_scale = Vector3(0.2f, 0.2f, 0.2f);
		pointsX.push_back(Math::RandomFloat(0.0f, 1.0f));
		pointsY.push_back(Math::RandomFloat(0.0f, 2.0f));
		//entity[i]->m_position = Math::RandomOnUnitSphere();
	}
}

void EditorState::RenderGeometry() {
	m_geometryShader->Bind();
	m_geometryShader->Set("_Iridescence", 5);
	m_geometryShader->Set("_Noise", 6);
	iri->Bind(5);
	noise->Bind(6);
	m_geometryShader->Set("_IridescenceStrength", 0);
	m_geometryShader->Set("_ProjectionMatrix", GetCamera()->GetProjectionMatrix());
	m_geometryShader->Set("_ViewMatrix", GetCamera()->GetViewMatrix());
	m_geometryShader->Set("_TransformationMatrix", Matrix4::Identity());
	m_geometryShader->Set("_CameraPosition", GetCamera()->m_position);
	//m_mori->Draw(m_geometryShader, GL_PATCHES);
	for (int i = 0; i < 64; i++) {
		entity[i]->Draw(m_geometryShader);
	}
}

void EditorState::Update(const TimeStep& time) {
	GetCamera()->Update(time);

	for (int i = 0; i < 64; i++) {
		//pointsX[i] += 0.002f;
		pointsY[i] += 0.05f * time.GetSeconds();
		if (pointsX[i] > 2.0f) pointsX[i] = 0.0f;
		if (pointsY[i] > 2.0f) pointsY[i] = 0.0f;
		entity[i]->m_position = Math::PointOnUnitSphere(pointsX[i], pointsY[i]);
	}
}

ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_NoDockingInCentralNode;
void EditorState::OnImGuiViewport() {
	ImGui::SetNextWindowDockID(m_dockspaceCenter, ImGuiCond_Always);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoTitleBar;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Viewport##1", nullptr, window_flags);

	if (ImGui::IsWindowFocused()) {
		ImGui::GetIO().WantCaptureMouse = false;
		ImGui::GetIO().WantCaptureKeyboard = false;
	}

	auto viewportSize = ImGui::GetContentRegionAvail();
	GetPipeline()->OnResize(viewportSize.x, viewportSize.y);
	GetFrameBufferManager()->OnResize(viewportSize.x, viewportSize.y);
	GetCamera()->UpdateProjectionMatrix();
	ImGui::Image((void*)GetPipeline()->GetFinalTexture()->GetHandle(), viewportSize, { 0, 1 }, { 1, 0 });
	ImGui::End();

	ImGui::SetNextWindowDockID(m_dockspaceLeft, ImGuiCond_Always);
	EditorState::CameraControls();
	ImGui::Begin("Editor", nullptr, window_flags);
	ImGui::End();

	ImGui::SetNextWindowDockID(m_dockspaceBottom, ImGuiCond_Always);
	Logger::OnImGui();
	ImGui::PopStyleVar(3);
}

void EditorState::CameraControls() {

}

void EditorState::OnImGUI() {

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	//window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", nullptr, window_flags);
	ImGui::PopStyleVar(3);

	// Dockspace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiID dockspace_id;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
		dockspace_id = ImGui::GetID("MyDockspace");

		if (!ImGui::DockBuilderGetNode(dockspace_id)) {
			ImGui::DockBuilderRemoveNode(dockspace_id);
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_None);

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