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
#include "imguiProfiler/Profiler.h"
#include "renderSyncManager.h"
#include "tests/test.h"
#include "glError.h"
#include "input/keyboard.h"
#include "input/mouse.h"
#include "metrics/metrics.h"

namespace emerald {
	static std::atomic<bool> g_running = true;
	static ResizeData g_resizeData;

	Application::Application(const ApplicationSettings& settings)
		: m_settings(settings) {
		App = this;
	}

	Application::~Application() {
		App = nullptr;
	}

	void Application::run() {
		GLFW::GLFWConfiguration config;
		GLFW::initialize(config);

		m_mainWindow = Ref<Window>::create(m_settings.m_name, m_settings.m_width, m_settings.m_height);
		m_mainWindow->makeContextCurrent();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			//LOG_ERROR("[GLAD] failed to initialize");
			return;
		}

		icon::loadIcon(m_mainWindow->handle());

		m_mainWindow->getCallbacks().addOnResizeCallback(this, &Application::onResize);
		m_mainWindow->getCallbacks().addOnKeyCallback(Keyboard::keyCallback);
		m_mainWindow->getCallbacks().addOnMouseButtonCallback(Mouse::mouseButtonCallback);
		m_mainWindow->getCallbacks().addOnMousePosCallback(Mouse::mousePosCallback);
		m_mainWindow->getCallbacks().addOnScrollCallback(Mouse::mouseScrollCallback);
		m_mainWindow->setVSync(true);
		m_mainWindow->setLimits(200, 60, GLFW_DONT_CARE, GLFW_DONT_CARE);

		//LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
		//LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

		ImGuiManager::initialize(m_mainWindow);


		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		GLError::setGLDebugMessageCallback();

		onInitialize();
		Renderer::flushCommandBufferOnThisThread();

		m_mainWindow->show();

		m_initialized = true;

		ThreadManager::registerCurrentThread(ThreadType::RENDER);

		Metrics::initialize();

		ThreadManager::createAndRegisterThread(ThreadType::LOGIC, "Logic", [this]() { logicLoop(); });

		renderLoop();

		close();

		Metrics::shutdown();
		ImGuiManager::shutdown();
		FrameBufferManager::shutdown();

		onShutdown();

		m_mainWindow.reset();
		glfwTerminate();
	}

	void Application::renderLoop() {
		PROFILE_REGISTER_RENDER_THREAD();
		while (g_running) {
			PROFILE_RENDER_FRAME();

			if (m_mainWindow->shouldClose()) {
				g_running = false;
			}

			PROFILE_RENDER_BEGIN("Process queue");
			processQueue();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("Wait for render buffer");
			Metrics::startTimer(Metric::RENDERWAIT);
			Renderer::acquireRenderBuffer();
			Metrics::endTimer(Metric::RENDERWAIT);
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("Render");
			Metrics::startTimer(Metric::RENDER);

			PROFILE_RENDER_BEGIN("PollEvents");
			m_mainWindow->pollEvents();
			PROFILE_RENDER_END();

			handleResize();

			Metrics::startTimer(Metric::GPU);
			PROFILE_RENDER_BEGIN("CommandBuffer");
			Renderer::executeCommandBuffer();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("SwapBuffers");
			m_mainWindow->swapBuffers();
			Metrics::endTimer(Metric::GPU);

			m_fpsCounter++;
			PROFILE_RENDER_END();

			Metrics::endTimer(Metric::RENDER);
			PROFILE_RENDER_END();
		}
	}

	void Application::logicLoop() {
		PROFILE_REGISTER_LOGIC_THREAD("Logic");
		while (g_running) {
			PROFILE_LOGIC_FRAME();

			PROFILE_LOGIC_BEGIN("Wait");
			Metrics::startTimer(Metric::LOGICWAIT);
			Renderer::waitForBufferAvailability();
			Metrics::endTimer(Metric::LOGICWAIT);
			PROFILE_LOGIC_END();

			PROFILE_LOGIC_BEGIN("Logic");
			Metrics::startTimer(Metric::LOGIC);
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - m_lastFrameTime;
			m_totalFrameTime += deltaTime;
			m_frameCount++;

			m_accumulatedTime += deltaTime;

			while (m_accumulatedTime >= m_fixedTimeStep) {
				fixedUpdate(Timestep(m_fixedTimeStep, m_totalFrameTime, m_frameCount));
				m_upsCounter++;
				m_accumulatedTime -= m_fixedTimeStep;
			}

			PROFILE_LOGIC_BEGIN("Input");
			Keyboard::update();
			Mouse::update();
			PROFILE_LOGIC_END();

			PROFILE_LOGIC_BEGIN("Update");
			update(Timestep(deltaTime, m_totalFrameTime, m_frameCount));
			PROFILE_LOGIC_END();

			//PROFILE_LOGIC_BEGIN("Index");
			//Sleep(20);
			//PROFILE_LOGIC_END();
			//
			//Renderer::submit([] {
			//	PROFILE_RENDER_BEGIN("Index");
			//	Sleep(20);
			//	PROFILE_RENDER_END();
			//});

			m_lastFrameTime = currentTime;

			if (currentTime - m_upsfpsCounter >= 1.0f) {
				m_fps = m_fpsCounter;
				m_ups = m_upsCounter;
				m_fpsCounter = 0;
				m_upsCounter = 0;
				m_upsfpsCounter = currentTime;
			}

			PROFILE_LOGIC_END();
			Metrics::endTimer(Metric::LOGIC);
			PROFILE_LOGIC_BEGIN("Submit buffer");
			Renderer::submitBufferForRendering();
			PROFILE_LOGIC_END();
		}
	}

	void Application::close() {
		glfwSetWindowShouldClose(m_mainWindow->handle(), true);
	}

	void Application::handleResize() {
		if (g_resizeData.m_shouldResize) {
			FrameBufferManager::onResize(g_resizeData.m_width, g_resizeData.m_height);
			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			GL(glViewport(0, 0, g_resizeData.m_width, g_resizeData.m_height));
			g_resizeData.m_shouldResize = false;
		}
	}

	void Application::onResize(uint32_t width, uint32_t height) {
		m_mainWindow->setWidth(width);
		m_mainWindow->setHeight(height);
		if (g_resizeData.m_width == width && g_resizeData.m_height == height)
			return;

		g_resizeData.m_width = width;
		g_resizeData.m_height = height;
		g_resizeData.m_shouldResize = true;
	}

	uint32_t Application::getWidth() const {
		return m_mainWindow->getWidth();
	}

	uint32_t Application::getHeight() const {
		return m_mainWindow->getHeight();
	}

	float Application::getTime() const {
		return (float)glfwGetTime();
	}

	void Application::processQueue() {
		static std::function<void()> func;
		while (m_eventQueue.tryToGet(func)) {
			func();
		}
	}
}