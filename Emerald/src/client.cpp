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
	//glfwWindowHint(GLFW_SAMPLES, 8);
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
	m_netHandler = NEW(NetHandlerClient(this));

	m_window->Show();

	while (!m_window->ShouldClose()) {
		GetGLFiberManager()->Tick();
	}
}

void Client::Run() {
	int updates = 0, frames = 0;
	float time = 0;
	while (!m_window->ShouldClose()) {
		m_updateTimer.Update();
		GetKeyboard()->Update();
		GetMouse()->Update();
		for (int i = 0; i < Math::Min(10, m_updateTimer.m_elapsedUpdates); i++) {
			GetMouse()->UpdateTick();
			Update(TimeStep(0.016f, m_updateTimer.m_lastTime, m_frameCount));
			updates++;
		}

		Render(m_updateTimer.m_partialUpdate);
		frames++;
		m_frameCount++;

		if (m_updateTimer.m_lastTime - time > 1000.0f) {
			m_window->SetTitle(Format_t("Emerald | UPS: %d FPS: %d", updates, frames));
			time = m_updateTimer.m_lastTime;
			m_fps = frames;
			updates = frames = 0;
		}

		GetGLFiberManager()->GoToMainFiber();
	}
}

void Client::Update(TimeStep time) {
	GetAssetManager()->Update();

	if (KeyJustUp('M')) {
		m_lockedMouse ^= true;
		glfwSetInputMode(GetClient()->GetWindow()->GetHandle(), GLFW_CURSOR, m_lockedMouse ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}
	auto& pCPUFrame = GetProfiler()->Start(ProfilerDataType::Update);
	GetStateManager()->Update(time);
	GetTweenManager()->Update(time);
	GetProfiler()->Update();

	if (m_world)
		m_world->Update(time);

	if (m_connectionState != ConnectionState::Disconnected) {
		static int TIMER = 0;
		Utils::DoTimedFunction(&TIMER, 50, [&] {
			PacketPlayerState packet;
			packet.salt = m_salt;
			packet.position = Camera::active->transform.position;
			packet.rotation = Camera::active->transform.rotation;
			packet.velocity = Camera::active->velocity;
			m_serverConnection.SendPacket(&packet);
		});

		NetEvent evnt;
		while (m_host.GetEvent(evnt)) {
			if (evnt.type == NetEventType::Data) {
				const void* data = evnt.packet;
				ReceivedPacket packet(evnt.packet);
				m_netHandler->HandlePacket(packet);
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

void Client::Render(float partialUpdate) {
	if (m_world)
		GetClient()->pipeline->directionalLight.SetTime(m_world->GetTime(), partialUpdate);

	auto& pRender = GetProfiler()->Start(ProfilerDataType::Render);
	auto& pGPUFrame = GetProfiler()->StartGL(ProfilerDataType::GPUFrame);
	if (!m_window->IsMinimized()) {
		m_window->ClearColor(Color(0, 0, 0, 1));

		pipeline->Render(partialUpdate);

		if (GetImGuiManager()->IsInitialized()) {
			//if (m_lockedMouse) ImGui::GetCurrentContext()->NavWindow = nullptr;
			GetImGuiManager()->Begin();
			if (UI::BeginWindow("Emerald", ImVec2(576, 680))) {
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
			//ImGui::ShowDemoWindow();
			GetProfiler()->OnGlobalImGui();

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

void Client::ConnectToServer(const String& IP, String_t name, int port) {
	if (m_connectionState != ConnectionState::Disconnected) return;
	ConnectClientToServer(m_host.m_handle, m_serverConnection, IP, port);
	m_connectionState = ConnectionState::Pending;
	PacketHandshake packet;
	packet.salt = m_salt;
	packet.SetName(name);
	packet.handshakeType = HandshakeType::Login;
	m_serverConnection.SendPacket(&packet);
}

void Client::SetBlock(const BlockPos& pos, uint8 block) {
	PacketBlockUpdate packet;
	packet.salt = m_salt;
	packet.blockType = block;
	packet.blockPosition = pos;
	m_serverConnection.SendPacket(&packet);
}

void Client::Disconnect() {
	if (m_connectionState != ConnectionState::Disconnected) {
		DisconnectClient(m_host.m_handle, m_serverConnection);
	}
	m_connectionState = ConnectionState::Disconnected;
}

void Client::Cleanup() {
	Disconnect();
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(m_netHandler);
	DELETE(pipeline);
	DELETE(m_world);
	DELETE(m_window);
	GetMemory()->CheckAllocations();
}