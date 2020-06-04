#include "stdafx.h"

String MenuState::GetDotDotDot(const String& text) {
	int timer = (int)(dotDotDotTimer / 180.0f);
	if (timer % 3 == 0) return text + ".";
	else if (timer % 3 == 1) return text + "..";
	else return text + "...";
}

void MenuState::Initialize() {
	Network::GetServerStatus("80.101.6.142", 25565, &m_status);
	srand((uint32)time(nullptr));
	String name = "Player_" + to_string(rand());
	strcpy(nameBuffer, name.c_str());
}

void MenuState::Update(const TimeStep& time) {
	dotDotDotTimer = time.TotalTime();

	if (GetClient()->m_world)
		GetClient()->m_world->SetTime(200);
}

void MenuState::Buttons() {
	ImGui::Separator();
	ImGui::PushItemWidth(200);

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f - 50));
	ImGui::Text("Server connection status:");
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f - 25));
	if (GetClient()->GetConnectionState() == ConnectionState::Connected) {
		ImGui::Text(GetDotDotDot("Connecting").c_str());
	} else {
		ImGui::Text(m_status.status == ServerStatusResult::Pending ? GetDotDotDot("Pending").c_str() : m_status.ToString().c_str());
	}

	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f));
	ImGui::InputText("Name", nameBuffer, 16);
	//ImGui::InputText("IP:", nameBuffer, 256, ImGuiInputTextFlags_CallbackCharFilter, ImGuiManager::IPFilter);

	ImGui::PushItemWidth(200);
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f + 25));
	bool dis = m_status.status != ServerStatusResult::Succesful;
	if (dis) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	if (ImGui::Button("Connect")) {
		//String str = buffer;
		//auto split = Utils::Split(str, ":");
		//if (split.size() != 2) return;
		//GetClient()->ConnectToServer(split[0], stoi(split[1].c_str()));
		GetClient()->ConnectToServer("80.101.6.142", nameBuffer, 25565);
	};

	if (dis) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	bool dis2 = m_status.status == ServerStatusResult::Pending;
	if (dis2) {
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f + 50));
	if (ImGui::Button("Refresh server status")) {
		//String str = buffer;
		//auto split = Utils::Split(str, ":");
		//if (split.size() != 2) {
		//	return;
		//	LOG("Invalid IP entered");
		//}
		//Network::GetServerStatus(split[0], stoi(split[1].c_str()), &m_status);
		Network::GetServerStatus("80.101.6.142", 25565, &m_status);
	}
	if (dis2) {
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}
	//ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f + 75));
	//if (ImGui::Button("Disconnect")) {
	//	//String str = buffer;
	//	//auto split = Utils::Split(str, ":");
	//	//if (split.size() != 2) return;
	//	//GetClient()->ConnectToServer(split[0], stoi(split[1].c_str()));
	//	GetClient()->Disconnect();
	//};
	ImGui::PopItemWidth();
	ImGui::PopItemWidth();
}

void MenuState::OnImGUI() {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("Dock", nullptr, window_flags);
	Buttons();
	ImGui::End();
	ImGui::PopStyleVar(3);
}