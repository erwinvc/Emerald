#include "eepch.h"
#include "application.h"
#include "engine/assets/core/assetRegistry.h"
#include "engine/events/events.h"
#include "engine/events/eventSystem.h"
#include "engine/input/keyboard.h"
#include "engine/input/mouse.h"
#include "graphics/textures/fallbackTextures.h"
#include "graphics/buffers/framebuffer.h"
#include "graphics/core/renderer.h"
#include "graphics/misc/DPI.h"
#include "graphics/misc/engineIcon.h"
#include "graphics/misc/glError.h"
#include "graphics/misc/glfw.h"
#include "graphics/window/window.h"
#include "imguiProfiler/Profiler.h"
#include "metrics/metrics.h"
#include "ui/imguiManager.h"
#include "utils/system/timestep.h"
#include "utils/threading/jobSystem.h"
#include "utils/threading/threadManager.h"
#include "utils/uuid/uuidGenerator.h"
#include "utils/system/time.h"

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
		EventSystem::setGlobalCallback(&Application::onEvent, this);
		GLFW::GLFWConfiguration config;
		GLFW::initialize(config);

		//tests::test();

		m_mainWindow = Ref<Window>::create(m_settings.m_name, m_settings.m_width, m_settings.m_height);
		m_mainWindow->makeContextCurrent();

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			//LOG_ERROR("[GLAD] failed to initialize");
			return;
		}

		icon::loadIcon(m_mainWindow->handle());

		Mouse::initialize();

		// #TODO: remove callbacks and move to 
		m_mainWindow->getCallbacks().addOnResizeCallback(this, &Application::onResize);
		m_mainWindow->getCallbacks().addOnKeyCallback(Keyboard::keyCallback);
		//m_mainWindow->getCallbacks().addOnMouseButtonCallback(Mouse::mouseButtonCallback);
		//m_mainWindow->getCallbacks().addOnMousePosCallback(Mouse::mousePosCallback);
		//m_mainWindow->getCallbacks().addOnScrollCallback(Mouse::mouseScrollCallback);
		m_mainWindow->setVSync(false);
		m_mainWindow->setLimits(200, 60, GLFW_DONT_CARE, GLFW_DONT_CARE);

		//LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
		//LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

		ImGuiManager::initialize(m_mainWindow);

		GL(glEnable(GL_DEBUG_OUTPUT));
		GL(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
		GLError::setGLDebugMessageCallback();

		Metrics::initialize();
		AssetRegistry::initialize();
		JobSystem::initialize(4);

		//ThreadManager::createAndRegisterThread(ThreadType::LOGIC, ProfilerThreadType::LOGIC, ThreadPriority::NORMAL, "Logic", [this]() { initializeLogic(); });
		ThreadManager::registerCurrentThread(ThreadType::MAIN);

		PROFILE_REGISTER_THREAD("Logic", ProfilerThreadType::LOGIC);

		FallbackTextures::initialize();

		onInitialize();

		//We want to render the first frame before showing the window
		loop();

		m_mainWindow->show();

		while (g_running) {
			loop();
		}

		FallbackTextures::shutdown();
		AssetRegistry::clear();
		//close();

		Metrics::shutdown();
		ImGuiManager::shutdown();
		FrameBufferManager::shutdown();

		onShutdown();

		m_mainWindow.reset();

		GLFW::terminate();
	}

	void Application::loop() {
		PROFILE_FRAME();

		if (m_mainWindow->shouldClose()) {
			g_running = false;
		}

		PROFILE_BEGIN("DPI check");
		DPI::update();
		PROFILE_END();

		PROFILE_BEGIN("Process queue");
		processQueue();
		PROFILE_END();

		PROFILE_BEGIN("PollEvents");
		m_mainWindow->pollEvents();
		PROFILE_END();

		handleResize();

		PROFILE_BEGIN("Logic");
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

		PROFILE_BEGIN("Input");
		Keyboard::update();
		Mouse::update();
		PROFILE_END();

		PROFILE_BEGIN("Process events");
		EventSystem::processEvents();
		PROFILE_END();

		PROFILE_BEGIN("Process queue");
		processQueueCPU();
		PROFILE_END();

		PROFILE_BEGIN("Asset streaming");
		AssetRegistry::update();
		PROFILE_END();

		PROFILE_BEGIN("Update");
		update(Timestep(deltaTime, m_totalFrameTime, m_frameCount));
		PROFILE_END();

		m_lastFrameTime = currentTime;

		if (currentTime - m_upsfpsCounter >= 1.0f) {
			m_fps = m_fpsCounter;
			m_ups = m_upsCounter;
			m_fpsCounter = 0;
			m_upsCounter = 0;
			m_upsfpsCounter = currentTime;
		}

		PROFILE_END();
		Metrics::endTimer(Metric::LOGIC);
		PROFILE_BEGIN("Submit buffer");

		PROFILE_BEGIN("SwapBuffers");
		m_mainWindow->swapBuffers();
		PROFILE_END();
		m_fpsCounter++;

		PROFILE_END();
	}

	void Application::close() {
		glfwSetWindowShouldClose(m_mainWindow->handle(), true);
	}

	void Application::handleResize() {
		if (g_resizeData.m_shouldResize) {
			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
			GL(glViewport(0, 0, g_resizeData.m_width, g_resizeData.m_height));
			EventSystem::dispatch<WindowResizeEvent>(g_resizeData.m_width, g_resizeData.m_height);
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
		while (m_eventQueue.tryGet(func)) {
			func();
		}
	}

	void Application::processQueueCPU() {
		static std::function<void()> func;
		while (m_eventQueueCPU.tryGet(func)) {
			func();
		}
	}

	bool Application::isFocused() const { return m_mainWindow->isFocussed(); }
}