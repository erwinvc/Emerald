#include "eepch.h"
#include "application.h"
#include "graphics/window.h"
#include "imgui.h"
#include "stdIncl.h"
#include "ui/imguiManager.h"
#include "util/timestep.h"
#include <format>
#include <util/color.h>
#include "GLFW/glfw3.h"
#include "graphics/engineIcon.h"
#include "graphics/texture.h"
#include "graphics/framebuffer.h"
#include "graphics/renderer.h"
#include "util/GLUtils.h"
#include "util/timer.h"
#include "glfw.h"

namespace emerald {
	Application::Application(const ApplicationSettings& settings) : m_settings(settings) {
		App = this;
	}

	Application::~Application() {

	}

	std::mutex mtx;
	std::condition_variable cv;

	bool keepRunning = true;
	bool renderFinished = false;
	auto MainThreadStart = std::chrono::high_resolution_clock::now();
	auto MainThreadEnd = std::chrono::high_resolution_clock::now();

	void Application::run() {
		GLFW::GLFWConfiguration config;
		GLFW::initialize(config);

		//m_mainWindow = std::make_unique<Window>(m_settings.m_name, m_settings.m_width, m_settings.m_height);
		m_mainWindow = Ref<Window>::create(m_settings.m_name, m_settings.m_width, m_settings.m_height);
		m_mainWindow->makeContextCurrent();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			//LOG_ERROR("[GLAD] failed to initialize");
			return;
		}

		icon::loadIcon(m_mainWindow->handle());

		m_mainWindow->getCallbacks().addOnResizeCallback(this, &Application::onResize);
		m_mainWindow->setVSync(true);
		m_mainWindow->setLimits(200, 60, GLFW_DONT_CARE, GLFW_DONT_CARE);

		//LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
		//LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

		imGuiManager::initialize(m_mainWindow);

		onInitialize();

		m_mainWindow->show();
		m_mainWindow->setVSync(false);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Makes debugging easier
		GLUtils::setDebugMessageCallback();

		threading::registerThread("Logic", [this]() { logicLoop(); });

		while (!m_mainWindow->shouldClose() && m_running) {
			renderLoop();
		}
		close();

		imGuiManager::shutdown();
		FrameBufferManager::shutdown();

		onShutdown();

		m_mainWindow.reset();
		glfwTerminate();
	}

	void Application::renderLoop() {
		auto renderThreadStart = std::chrono::high_resolution_clock::now();

		m_mainWindow->pollEvents();
		Renderer::executeCommandBuffer();
		m_mainWindow->swapBuffers();
		renderFinished = true;
		cv.notify_all();

		auto renderThreadEnd = std::chrono::high_resolution_clock::now();
		auto renderDuration = std::chrono::duration_cast<std::chrono::milliseconds>(renderThreadEnd - renderThreadStart);
		//Log::info("Render thread {}ms", renderDuration.count());
	}

	void Application::logicLoop() {
		while (m_running) {
			MainThreadStart = std::chrono::high_resolution_clock::now();

			std::unique_lock<std::mutex> lock(mtx);
			cv.wait(lock, [] { return renderFinished; });
			renderFinished = false;

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

			update(Timestep(deltaTime, m_totalFrameTime, m_frameCount));
			m_lastFrameTime = currentTime;

			if (currentTime - m_lastTitleUpdateTime >= 1.0f) {
				Log::info("{} | UPS: {} FPS: {}", m_settings.m_name, m_ups, m_fps);

				m_fps = 0;
				m_ups = 0;
				m_lastTitleUpdateTime = currentTime;
			}

			MainThreadEnd = std::chrono::high_resolution_clock::now();
			auto MainThreadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(MainThreadEnd - MainThreadStart);
			//Log::info("Main thread {}ms", MainThreadDuration.count());
		}
	}

	void Application::close() {
		m_running = false;
	}

	void Application::onResize(uint32_t width, uint32_t height) {
		Renderer::submit([width, height, this] {
			FrameBufferManager::onResize(width, height);
			glViewport(0, 0, width, height);
			update(Timestep(0, m_totalFrameTime, m_frameCount));
			m_mainWindow->swapBuffers();
		});
		Renderer::executeCommandBuffer();
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
}