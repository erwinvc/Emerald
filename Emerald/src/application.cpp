#include "stdafx.h"

static void ErrorCallback(int error, const char* description) {
	LOG_ERROR("[GLFW] %s", description);
}

Application::Application() : m_running(true) {
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
	m_window->Show();
	m_window->ClearColor(Color(0.5f, 0.7f, 1.0f, 1.0f));

	if (glewInit() != GLEW_OK) {
		LOG_ERROR("GLEW failed to initialize");
		return;
	}

	GetTextureManager()->Initialize();
	GetMaterialManager()->Initialize();
	GetGLCallbackManager()->AddOnResizeCallback(this, &Application::OnResize);
	GetGLCallbackManager()->AddOnCloseCallback(this, &Application::OnWindowClose);
}


void Application::OnResize(int width, int height) {
	glViewport(0, 0, width, height);
	m_pipeline->Resize(width, height);
}

void Application::OnWindowClose() {
	m_running = false;
}

void Application::Run() {

	m_pipeline = NEW(RenderingPipeline());
	GetKeyboard()->Initialize(m_window);
	GetMouse()->Initialize(m_window);
	GetImGuiManager()->Initialize(m_window);

	glEnable(GL_DEPTH_TEST);

	m_window->SetVSync(false);

	/*Main loop*/
	m_timer = Timer();
	m_timeStep = TimeStep(m_timer.Get());
	long timer = m_timer.Get();
	float updateTimer = m_timer.Get();
	float updateTick = 1000.0f / 60.0f;
	double delta = 0;
	int frames = 0, updates = 0;
	while (m_running) {
		m_window->Clear();
		float now = m_timer.Get();
		if (now - updateTimer > updateTick) {
			m_timeStep.Update(now);
			Update(m_timeStep);
			updates++;
			updateTimer += updateTick;
			m_frameCount++;
		}
		delta += (now - updateTimer) / updateTick;
		//Update(delta);
		//while (delta >= 1.0) {
		//    FixedUpdate();
		//    updates++;
		//    delta--;
		//}
		Render();
		frames++;
		if (glfwGetTime() - timer > 1.0) {
			m_window->SetTitle(Format_t("Emerald | UPS: %d FPS: %d", updates, frames));
			timer++;
			updates = frames = 0;
		}
	}
}
void Application::FixedUpdate() {
}

void Application::Update(const TimeStep& time) {
	GetMouse()->Update();
	m_pipeline->Update(time);
}

void Application::Render() {

	m_window->ClearColor(Color(0, 0, 0, 1));
	GetImGuiManager()->Begin();

	m_pipeline->Render();

	GetImGuiManager()->End();

	m_window->SwapBuffers();
	m_window->PollEvents();
}

void Application::Cleanup() {
	GetThreadManager()->Cleanup();
	DELETE(m_window);
	DELETE(m_pipeline);
	GetMemory()->CheckAllocations();
	//delete m_timer;
	//delete m_timeStep;
	//delete m_pipeline;
}