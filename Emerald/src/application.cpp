#include "stdafx.h"

glm::ivec2 toResize(-1, -1);

static void ErrorCallback(int error, const char* description) {
	LOG_ERROR("[GLFW] %s", description);
}

static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	LOG("%d %d %s", type, id, message);
}

void Application::OnResize(int width, int height) {
	toResize = glm::ivec2(width, height);
}

void Application::OnWindowClose() {
	m_running = false;
}

void Application::Initialize() {
	//glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
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

	//m_hwndHandle = glfwGetWin32Window(m_window->GetHandle());

	//glEnable(GL_DEBUG_OUTPUT);
	//glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	//glDebugMessageCallback(DebugCallback, nullptr);

	GetGLCallbackManager()->AddOnCloseCallback(this, &Application::OnWindowClose);

	LOG("[~cGPU~x] %-26s %s", "GPU manufacturer~1", glGetString(GL_VENDOR));
	LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
	LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

	CapabilitiesCheck();


	m_window->SetIcon(Icon("icon32"));

	GetStateManager()->RegisterStates();
	pipeline = NEW(HDRPipeline());
	pipeline->EarlyInitialize(m_window->GetWidth(), m_window->GetHeight());
	
	GetGLFiberManager()->Initialize();
	GetGLFiberManager()->AddFiber("Main", [] {GetApp()->Run(); });
	GetGLFiberManager()->AddFiber("AssetManager", [] {GetAssetManager()->Update(); });

	m_window->Show();

	GetGLCallbackManager()->AddOnResizeCallback(this, &Application::OnResize);

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
			m_totalFrameTime += time - m_lastFrameTime;
			Update(TimeStep(time - m_lastFrameTime, m_totalFrameTime, m_frameCount));
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

void Application::HandleQueue() {
	function<void()> task;
	if (m_queue.TryToGet(task)) {
		task();
	}
}

void Application::Render() {
	m_window->ClearColor(Color(0, 0, 0, 1));


	pipeline->Render();

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

void Application::CapabilitiesCheck() {
	int maxSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);

	GLint max_layers;
	glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_layers);

	//MessageBox(hWnd, "id is: <int id here?>", "Msg title", MB_OK | MB_ICONQUESTION);
}

void Application::Cleanup() {
	GetStateManager()->Cleanup();
	GetThreadManager()->Cleanup();
	DELETE(pipeline);
	DELETE(m_window);
	GetMemory()->CheckAllocations();
}