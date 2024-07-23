#include "eepch.h"
#include "application.h"
#include "graphics/window.h"
#include "imgui.h"
#include "stdIncl.h"
#include "threading/fiberManager.h"
#include "ui/imguiManager.h"
#include "util/timestep.h"
#include <format>
#include <util/color.h>
#include "GLFW/glfw3.h"

namespace emerald {
	void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

		Log::info("GL CALLBACK: type = 0x{0:x}, severity = 0x{1:x}, message = {2}", type, severity, message);
	}

	Application::Application(const ApplicationSettings& settings) : m_name(settings.m_name) {
		App = this;
	}

	Application::~Application() {

	}
	static void glfw_error_callback(int error, const char* description) {
		Log::info("GLFW Error {0}: {1}", error, description);
		//fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}
	void Application::run() {
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit()) {
			//LOG_ERROR("[GLFW] GLFW failed to initialize");
			return;
		}

		
		glfwDefaultWindowHints();
		//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

		glfwWindowHint(0x00C2000D, false);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		//glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);

#ifdef EE_DEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		m_mainWindow = new Window("Diamond", 1920, 1080);
		m_mainWindow->makeContextCurrent();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			//LOG_ERROR("[GLAD] failed to initialize");
			return;
		}

		m_mainWindow->getCallbacks().addOnResizeCallback(this, &Application::onResize);
		m_mainWindow->clearColor(Color(0.5f, 0.7f, 1.0f, 1.0f));
		m_mainWindow->setVSync(true);
		m_mainWindow->setLimits(200, 60, GLFW_DONT_CARE, GLFW_DONT_CARE);

		//LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
		//LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

		imGuiManager::initialize(m_mainWindow);

		FiberManager::initializeMainFiber();

		FiberManager::registerFiber("Gameloop", FiberLifetime::LOOPING, [this]() {this->loop(); });

		m_mainWindow->show();
		m_mainWindow->setVSync(false);


		// During your OpenGL initialization
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes debugging easier
		//glDebugMessageCallback(MessageCallback, 0);

		onInitialize();

		while (!m_mainWindow->shouldClose() && m_running) {
			FiberManager::run();
		}

		imGuiManager::shutdown();

		delete m_mainWindow;
		glfwTerminate();
	}

	void Application::loop() {
		float currentTime = (float)glfwGetTime();
		float deltaTime = currentTime - m_lastFrameTime;
		m_totalFrameTime += deltaTime;
		m_frameCount++;
		m_fps++;

		accumulatedTime += deltaTime;

		static std::function<void()> func;
		while (m_eventQueue.tryToGet(func)) {
			func();
		}

		while (accumulatedTime >= m_fixedTimeStep) {
			fixedUpdate(Timestep(m_fixedTimeStep, m_totalFrameTime, m_frameCount));
			m_ups++;
			accumulatedTime -= m_fixedTimeStep;
		}

		m_mainWindow->clear();

		update(Timestep(deltaTime, m_totalFrameTime, m_frameCount));
		m_lastFrameTime = currentTime;

		m_mainWindow->swapBuffers();

		if (currentTime - m_lastTitleUpdateTime >= 1.0f) {
			m_mainWindow->setTitle(std::format("{} | UPS: {} FPS: {}", m_name, m_ups, m_fps));
			m_fps = 0;
			m_ups = 0;
			m_lastTitleUpdateTime = currentTime;
		}

		m_mainWindow->pollEvents();

	}

	void Application::close() {
		m_running = false;
	}

	void Application::onResize(uint32_t with, uint32_t height) {
		glViewport(0, 0, with, height);
		m_mainWindow->clear();
		update(Timestep(0, m_totalFrameTime, m_frameCount));
		m_mainWindow->swapBuffers();
	}

	uint32_t Application::getWidth() const {
		return m_mainWindow->getWidth();
	}

	uint32_t Application::getHeight() const {
		return m_mainWindow->getHeight();
	}

	double Application::getTime() const {
		return glfwGetTime();
	}

	Application* App;
}
//		m_pipeline->PreGeometryRender();
//
//		GetLineRenderer()->Begin();
//		GetStateManager()->RenderGeometry();
//		GetLineRenderer()->End();
//		GetLineRenderer()->Draw();
//
//		m_pipeline->PostGeometryRender();
//
//		m_pipeline->PreUIRender();
//		GetStateManager()->RenderUI();
//		m_pipeline->PostUIRender();
//
//		GetImGuiManager()->Begin();
//		if (ImGui::Begin("Emerald###Window", &m_ImGuiOpen)) {
//			if (ImGui::BeginTabBar("Tab", ImGuiTabBarFlags_NoCloseWithMiddleMouseButton)) {
//				m_pipeline->OnImGUI();
//				GetStateManager()->OnImGUI();
//				GetFrameBufferManager()->OnImGUI();
//				GetShaderManager()->OnImGUI();
//				ImGui::EndTabBar();
//			}
//			ImGui::End();
//		}
//		GetImGuiManager()->End();
//	} else {
//		GLUtils::EnableBlending();
//		GetStateManager()->RenderUI();
//		GLUtils::DisableBlending();
//	}
//