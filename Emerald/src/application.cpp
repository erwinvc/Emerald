#include "stdafx.h"

Vector2I toResize(-1, -1);

static void ErrorCallback(int error, const char* description) {
	LOG_ERROR("[GLFW] %s", description);
}

void Application::OnResize(int width, int height) {
	if (!m_initialized) return;
	toResize = Vector2I(width, height);
}

void Application::OnWindowClose() {
	m_running = false;
}

void Application::Initialize() {
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

	GetGLCallbackManager()->AddOnResizeCallback(this, &Application::OnResize);
	GetGLCallbackManager()->AddOnCloseCallback(this, &Application::OnWindowClose);

	LOG("[~cGPU~x] %-26s %s", "GPU manufacturer~1", glGetString(GL_VENDOR));
	LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
	LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

	m_pipeline = NEW(RenderingPipeline());

	m_window->SetIcon(Icon("icon32"));

	GetStateManager()->RegisterStates();

	GetGLFiberManager()->Initialize();
	GetGLFiberManager()->AddFiber("Main", [] {GetApplication()->Run(); });
	GetGLFiberManager()->AddFiber("AssetManager", [] {GetAssetManager()->Update(); });

	m_window->Show();

	m_initialized = true;


	while (m_running) {
		GetGLFiberManager()->Tick();
	}
}

void Application::Run() {
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
			Update(time - m_lastFrameTime);
			m_lastFrameTime = time;
			updates++;
			updateTimer += updateTick;
			m_frameCount++;
		}
		delta += (time - updateTimer) / updateTick;
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

void Application::Update(TimeStep time) {
	GetMouse()->Update();
	GetStateManager()->Update(time);
	GetTweenManager()->Update(time);
	GetShaderManager()->Update(time);
}

void Application::Render() {
	m_window->ClearColor(Color(0, 0, 0, 1));

	if (m_pipeline->Initialized()) {
		m_pipeline->PreGeometryRender();

		GetLineRenderer()->Begin();
		GetStateManager()->RenderGeometry();
		GetLineRenderer()->End();
		GetLineRenderer()->Draw();

		m_pipeline->PostGeometryRender();

		m_pipeline->PreUIRender();
		GetStateManager()->RenderUI();
		m_pipeline->PostUIRender();

		GetImGuiManager()->Begin();
		if (ImGui::Begin("Emerald###Window", &m_ImGuiOpen, ImVec2(576, 680), -1)) {
			if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
				m_pipeline->OnImGUI();
				GetStateManager()->OnImGUI();
				GetShaderManager()->OnImGUI();
				ImGui::EndTabBar();
			}
			ImGui::End();
		}
		GetImGuiManager()->End();
	} else {
		GLUtils::EnableBlending();
		GetStateManager()->RenderUI();
		GLUtils::DisableBlending();
	}

	if (toResize.x != -1) {
		uint width = toResize.x;
		uint height = toResize.y;
		glViewport(0, 0, width, height);
		m_window->SetWidth(width);
		m_window->SetHeight(height);
		m_pipeline->OnResize(width, height);
		float aspect = (float)(width) / height;
		GetCamera()->SetProjectionMatrix(70, aspect, 0.01f, 1000.0f);
		//GetStateManager()->OnResize(width, height);
		toResize = Vector2I(-1, -1);
	}

	m_window->SwapBuffers();
	m_window->PollEvents();
}

void Application::Cleanup() {
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(m_window);
	DELETE(m_pipeline);
	GetMemory()->CheckAllocations();
}