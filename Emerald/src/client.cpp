#include "stdafx.h"

//uint Application::s_globalLogValue = 0;



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

	//GetGLCallbackManager()->AddOnCloseCallback(this, &Client::OnWindowClose);

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

	GetGLCallbackManager()->AddOnResizeCallback(m_window, &Window::OnResize);

	m_initialized = true;

	glfwSetWindowSize(m_window->GetHandle(), 1920, 1080);

	m_window->Show();

	while (!m_window->ShouldClose()) {
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
	while (!m_window->ShouldClose()) {
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

static float timer = 0;
static float lastTimer = 0;
void Client::Update(TimeStep time) {
	auto& pCPUFrame = GetProfiler()->Start(ProfilerDataType::Update);
	GetMouse()->Update();
	GetStateManager()->Update(time);
	GetTweenManager()->Update(time);
	GetShaderManager()->Update(time);
	GetProfiler()->Update();

	if (m_connectionState != ConnectionState::Disconnected) {
		static int TIMER = 0;
		Utils::DoTimedFunction(&TIMER, 50, [&] {
			PacketPlayerState packet;
			packet.salt = m_salt;
			packet.position = Camera::active->transform.position;
			packet.rotation = Camera::active->transform.rotation;
			m_serverConnection.SendPacket(&packet);
		});

		NetEvent evnt;
		while (m_host.GetEvent(evnt)) {
			if (evnt.type == NetEventType::Data) {
				const void* data = evnt.packet;
				ReceivedPacket packet(evnt.packet);
				HandlePacket(packet);
				evnt.Destroy();
			}
		}
	}
	pCPUFrame.End();
}

void Client::HandleQueue() {
	function<void()> task;
	if (m_queue.TryToGet(task)) {
		task();
	}
}

void Client::Render() {
	auto& pRender = GetProfiler()->Start(ProfilerDataType::Render);
	auto& pGPUFrame = GetProfiler()->StartGL(ProfilerDataType::GPUFrame);
	if (!m_window->IsMinimized()) {
		m_window->ClearColor(Color(0, 0, 0, 1));

		pipeline->Render();

		if (GetImGuiManager()->IsInitialized()) {
			GetImGuiManager()->Begin();

			if (UI::BeginWindow("Emerald")) {
				if (ImGui::Button("Disconnect")) {
					GetClient()->Disconnect();
					GameStates::VOXEL->GetWorld()->Clear();
					GetStateManager()->SetState(GameStates::MENU);
				}
				if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
					pipeline->OnImGUI();
					GetStateManager()->OnStateImGUI();
					GetFrameBufferManager()->OnImGUI();
					GetShaderManager()->OnImGUI();
					GetProfiler()->OnImGui();
					ImGui::EndTabBar();
				}
			}
			UI::EndWindow();

			GetStateManager()->OnImGUI();
			GetImGuiManager()->End();
		}
	}

	if (m_window->CheckResized()) {
		uint width = m_window->GetWidth();
		uint height = m_window->GetHeight();
		glViewport(0, 0, width, height);
		pipeline->OnResize(width, height);
		GetFrameBufferManager()->OnResize(width, height);
		GetStateManager()->OnResize(width, height);
	}

	GetAssetWatcher()->HandleQueue();
	HandleQueue();

	m_window->SwapBuffers();
	m_window->PollEvents();
	
	pGPUFrame.End();
	pRender.End();
}

void Client::ConnectToServer(const String& IP, int port) {
	if (m_connectionState != ConnectionState::Disconnected) return;
	ConnectClientToServer(m_host.m_handle, m_serverConnection, IP, port);
	m_connectionState = ConnectionState::Pending;
	PacketHandshake packet;
	packet.salt = m_salt;
	packet.handshakeType = HandshakeType::Login;
	m_serverConnection.SendPacket(&packet);
}

void Client::Disconnect() {
	if (m_connectionState != ConnectionState::Disconnected) {
		DisconnectClient(m_host.m_handle, m_serverConnection);
	}
	m_connectionState = ConnectionState::Disconnected;
}

void Client::HandlePacket(const ReceivedPacket& packet) {
	switch (packet.type) {
		case PacketType::HandshakeChallenge: OnHandshakeChallenge(packet.As<PacketHandshakeChallenge>()); break;
		case PacketType::ConnectionResult: OnConnectionResult(packet.As<PacketConnectionResult>()); break;
		case PacketType::Kick: OnKick(packet.As<PacketKick>()); break;
		case PacketType::GameData: OnGameData(packet.GetData()); break;
		case PacketType::AddEntity: OnAddEntity(packet.GetData()); break;
		case PacketType::RemoveEntity: OnRemoveEntity(packet.As<PacketRemoveEntity>()); break;
		case PacketType::UpdateEntities: OnUpdateEntities(packet.GetData()); break;
		case PacketType::BlockUpdate: OnBlockUpdate(packet.As<PacketBlockUpdate>()); break;
	}
}

void Client::OnHandshakeChallenge(const PacketHandshakeChallenge& packet) {

	uint32 newSalt = m_salt ^ packet.salt;
	m_salt = newSalt;

	PacketHandshakeResponse newPacket;
	newPacket.salt = m_salt;
	m_serverConnection.SendPacket(&newPacket);

	LOG("[~cNetwork~x] received handshake challenge");
	m_connectionState = ConnectionState::Connected;
}

void Client::CapabilitiesCheck() {
	int maxSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

	GLint max_layers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);

	//MessageBox(hWnd, "id is: <int id here?>", "Msg title", MB_OK | MB_ICONQUESTION);
}

void Client::OnConnectionResult(const PacketConnectionResult& packet) {
	if (packet.result) {
		LOG("[~cNetwork~x] connection accepted");
		m_connectionState = ConnectionState::Connected;
		m_playerID = packet.playerID;
	} else {
		LOG("[~cNetwork~x] connection declined: %s", packet.reason);
		Disconnect();
	}
}

void Client::OnKick(const PacketKick& packet) {
	LOG("[~cNetwork~x] kicked! %s", packet.reason);
	GetClient()->Disconnect();
	GameStates::VOXEL->GetWorld()->Clear();
	GetStateManager()->SetState(GameStates::MENU);
}

void Client::OnGameData(const void* data) {
	LOG("[~cNetwork~x] received chunk data");

	PacketReader reader(data);
	PacketType type = reader.type;
	uint32 chunkCount = reader.Read<uint32>();
	LOG("[~cNetwork~x] %u chunks", chunkCount);

	uint8 blockData[CHUNK_VOLUME];
	for (uint32 i = 0; i < chunkCount; i++) {
		ClientWorld* world = GameStates::VOXEL->GetWorld();
		glm::ivec3 pos = reader.Read<glm::ivec3>();
		for (int j = 0; j < CHUNK_VOLUME; j++) {
			blockData[j] = reader.Read<uint8>();
		}
		world->m_chunks.emplace(pos, Chunk(pos, blockData));
	}

	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 id = reader.Read<uint32>();
		auto position = reader.Read<glm::vec3>();
		auto rotation = reader.Read<glm::vec3>();
		GameStates::VOXEL->GetWorld()->AddEntity(id, position, rotation);
	}

	GetStateManager()->SetState(GameStates::VOXEL);
}

void Client::OnAddEntity(const void* data) {
	LOG("[~cNetwork~x] added entity");

	PacketReader reader(data);
	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 entityId = reader.Read<uint32>();
		glm::vec3 position = reader.Read<glm::vec3>();
		glm::vec3 rotation = reader.Read<glm::vec3>();

		GameStates::VOXEL->GetWorld()->AddEntity(entityId, position, rotation);
	}
}

void Client::OnRemoveEntity(const PacketRemoveEntity& packet) {
	LOG("[~cNetwork~x] removed entity");

	GameStates::VOXEL->GetWorld()->RemoveEntity(packet.entity);
}

void Client::OnUpdateEntities(const void* data) {
	PacketReader reader(data);
	uint32 count = reader.Read<uint32>();
	for (uint32 i = 0; i < count; i++) {
		uint32 entityId = reader.Read<uint32>();
		glm::vec3 position = reader.Read<glm::vec3>();
		glm::vec3 rotation = reader.Read<glm::vec3>();
		GameStates::VOXEL->GetWorld()->UpdateEntity(entityId, position, rotation);
	}
}

void Client::SetBlock(const BlockPos& pos, uint8 block) {
	PacketBlockUpdate packet;
	packet.salt = m_salt;
	packet.blockType = block;
	packet.blockPosition = pos;
	m_serverConnection.SendPacket(&packet);
}

void Client::OnBlockUpdate(const PacketBlockUpdate& packet) {
	GameStates::VOXEL->GetWorld()->SetBlock(packet.blockPosition, packet.blockType);
}

void Client::Cleanup() {
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(pipeline);
	DELETE(m_window);
	GetMemory()->CheckAllocations();
}