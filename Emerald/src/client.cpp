#include "stdafx.h"

//uint Application::s_globalLogValue = 0;

glm::ivec2 toResize(-1, -1);

void Client::OnResize(int width, int height) {
	toResize = glm::ivec2(width, height);
}

void Client::OnWindowClose() {
	m_running = false;
}

void Client::Initialize() {
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		LOG_ERROR("[GLFW] GLFW failed to initialize");
		return;
	}

	glfwDefaultWindowHints();
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);

	m_window = NEW(Window("Emerald", 1920, 1080));

	m_window->MakeContextCurrent();
	m_window->ClearColor(Color(0.5f, 0.7f, 1.0f, 1.0f));
	m_window->SetVSync(true);

	if (glewInit() != GLEW_OK) {
		LOG_ERROR("[GLEW] failed to initialize");
		return;
	}

	//glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback(DebugCallback, nullptr);

	GetGLCallbackManager()->AddOnCloseCallback(this, &Client::OnWindowClose);

	LOG("[~cGPU~x] %-26s %s", "GPU manufacturer~1", glGetString(GL_VENDOR));
	LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
	LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

	m_window->SetIcon(Icon("icon32"));

	GetStateManager()->RegisterStates();
	pipeline = NEW(HDRPipeline());
	pipeline->EarlyInitialize(GetWidth(), GetHeight());

	GetGLFiberManager()->Initialize();
	GetGLFiberManager()->AddFiber("Main", [] {GetClient()->Run(); });
	GetGLFiberManager()->AddFiber("AssetManager", [] {GetAssetManager()->Update(); });

	m_window->Show();

	GetGLCallbackManager()->AddOnResizeCallback(this, &Client::OnResize);

	m_initialized = true;

	ConnectClientToServer(m_host.m_handle, m_serverConnection, "80.101.6.142", 25565);
	m_connectionState = ConnectionState::Pending;

	PacketHandshakeOne packet;
	packet.salt = m_salt;
	m_serverConnection.Send(&packet, sizeof(PacketHandshakeOne), 0, ENET_PACKET_FLAG_RELIABLE);

	while (m_running) {
		GetGLFiberManager()->Tick();
	}
}

void Client::Run() {
	m_timer = Timer();
	float timer = m_timer.Get();
	float updateTimer = m_timer.Get();
	float updateTick = 1000.0f / 60.0f;
	double delta = 0;
	int frames = 0, updates = 0;
	while (m_running) {
		m_window->Clear();
		float time = m_timer.Get();
		if (time - updateTimer > updateTick) {
			m_totalFrameTime += time - m_lastFrameTime;
			Update(TimeStep(time - m_lastFrameTime, m_totalFrameTime, m_frameCount));

			m_lastFrameTime = time;
			updates++;
			updateTimer += updateTick;
			m_frameCount++;
		}
		delta += (time - updateTimer) / updateTick;
		//LOG("GlobalLogValue: %d", s_globalLogValue);
		//s_globalLogValue = 0;
		Render();
		frames++;
		if (glfwGetTime() - timer > 1.0) {
			m_window->SetTitle(Format_t("Emerald | UPS: %d FPS: %d", updates, frames));
			m_fps = frames;
			timer++;
			updates = frames = 0;
		}
		GetGLFiberManager()->GoToMainFiber();
	}
}

void Client::Update(TimeStep time) {
	NetEvent evnt;
	while (m_host.GetEvent(evnt)) {
		if (evnt.type == NetEventType::Data) {
			const void* data = evnt.packet;
			ReceivedPacket packet(evnt.packet);
			HandlePacket(packet);
			evnt.Destroy();
		}
	}

	GetMouse()->Update();
	GetStateManager()->Update(time);
	GetTweenManager()->Update(time);
	GetShaderManager()->Update(time);
}

void Client::HandleQueue() {
	function<void()> task;
	if (m_queue.TryToGet(task)) {
		task();
	}
}

void Client::Render() {
	m_window->ClearColor(Color(0, 0, 0, 1));

	pipeline->Render();

	if (GetImGuiManager()->IsInitialized()) {
		GetImGuiManager()->Begin();

		if (UI::BeginWindow("Emerald")) {
			if (UI::Button("Connect to server")) {
			}
			if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
				pipeline->OnImGUI();
				GetStateManager()->OnStateImGUI();
				GetFrameBufferManager()->OnImGUI();
				GetShaderManager()->OnImGUI();
				ImGui::EndTabBar();
			}
		}
		UI::EndWindow();

		GetStateManager()->OnImGUI();
		GetImGuiManager()->End();
	}

	if (toResize.x != -1) {
		uint width = toResize.x;
		uint height = toResize.y;
		glViewport(0, 0, width, height);
		m_window->SetWidth(width);
		m_window->SetHeight(height);
		pipeline->OnResize(width, height);
		GetFrameBufferManager()->OnResize(width, height);
		GetStateManager()->OnResize(width, height);
		toResize = glm::ivec2(-1, -1);
	}

	GetAssetWatcher()->HandleQueue();
	HandleQueue();

	m_window->SwapBuffers();
	m_window->PollEvents();
}

void Client::HandlePacket(const ReceivedPacket& packet) {
	switch (packet.type) {
		case PacketType::HandshakeChallenge: OnHandshakeChallenge(packet.As<PacketHandshakeChallenge>()); break;
		case PacketType::ConnectionAccepted: OnConnectionAccepted(packet.As<PacketConnectionAccepted>()); break;
		case PacketType::Kick: OnKick(packet.As<PacketKick>()); break;
		case PacketType::GameData: OnGameData(packet.As<PacketGameData>()); break;
		case PacketType::AddEntity: OnAddEntity(packet.As<PacketAddEntity>()); break;
		case PacketType::RemoveEntity: OnRemoveEntity(packet.As<PacketRemoveEntity>()); break;
		case PacketType::UpdateEntities: OnUpdateEntities(packet.As<PacketUpdateEntities>()); break;
		case PacketType::BlockUpdate: OnBlockUpdate(packet.As<PacketBlockUpdate>()); break;
	}
}

void Client::OnHandshakeChallenge(const PacketHandshakeChallenge& packet) {
	uint32 newSalt = m_salt ^ packet.salt;
	m_salt = newSalt;

	PacketHandshakeResponse newPacket;
	newPacket.salt = m_salt;
	m_serverConnection.SendPacket(&newPacket, 0, ENET_PACKET_FLAG_RELIABLE);

	LOG("[~cNetwork~x] received handshake challenge");
}

void Client::CapabilitiesCheck() {
	int maxSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

	GLint max_layers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);

	//MessageBox(hWnd, "id is: <int id here?>", "Msg title", MB_OK | MB_ICONQUESTION);
}

void Client::OnConnectionAccepted(const PacketConnectionAccepted& packet) {

}

void Client::OnKick(const PacketKick& packet) {

}

void Client::OnGameData(const PacketGameData& packet) {

}

void Client::OnAddEntity(const PacketAddEntity& packet) {

}

void Client::OnRemoveEntity(const PacketRemoveEntity& packet) {

}

void Client::OnUpdateEntities(const PacketUpdateEntities& packet) {

}

void Client::OnBlockUpdate(const PacketBlockUpdate& packet) {

}

void Client::Cleanup() {
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(m_window);
	DELETE(pipeline);
	GetMemory()->CheckAllocations();
}