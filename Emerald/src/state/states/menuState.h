#pragma once

struct Position {
	glm::vec3 m_position;
};
class MenuState : public State {
private:

public:
	MenuState() : State("Menu") {}
	ServerStatus m_status;
	float dotDotDotTimer = 0;

	void Initialize() override {
		Network::GetServerStatus("80.101.6.142", 25565, &m_status);
	}

	void Update(const TimeStep& time) override {
		dotDotDotTimer = time.TotalTime();
	}
	
	void RenderGeometry(HDRPipeline* pipeline) override {
	}

	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override {
	}

	void OnStateImGUI() override {

	}

	void OnImGUI() override {
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
		Buttons();
		ImGui::End();
		ImGui::PopStyleVar(3);
	}

	String GetDotDotDot(const String& text) {
		int timer = (int)(dotDotDotTimer / 180.0f);
		if (timer % 3 == 0) return text + ".";
		else if (timer % 3 == 1) return text + "..";
		else return text + "...";
	}

	void Buttons() {
		static char buffer[256];
		ImGui::Separator();
		ImGui::PushItemWidth(200);

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f - 50));
		ImGui::Text("Server connection status:");
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f - 25));
		if(GetClient()->GetConnectionState() == ConnectionState::Connected)
		{
			ImGui::Text(GetDotDotDot("Connecting").c_str());
		} else {
			ImGui::Text(m_status.status == ServerStatusResult::Pending ? GetDotDotDot("Pending").c_str() : m_status.ToString().c_str());
		}

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f));
		//ImGui::InputText("IP Address", buffer, 256, ImGuiInputTextFlags_CallbackCharFilter, ImGuiManager::IPFilter);

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
			GetClient()->ConnectToServer("80.101.6.142", 25565);
		};

		if (dis) {
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
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

		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x * 0.5f - 100, ImGui::GetWindowSize().y * 0.5f + 75));
		if (ImGui::Button("Disconnect")) {
			//String str = buffer;
			//auto split = Utils::Split(str, ":");
			//if (split.size() != 2) return;
			//GetClient()->ConnectToServer(split[0], stoi(split[1].c_str()));
			GetClient()->Disconnect();
		};
		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
	}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};