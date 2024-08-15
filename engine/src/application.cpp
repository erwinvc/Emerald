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

namespace emerald {
	static std::mutex g_mtx;
	static std::condition_variable g_cvRender, g_cvLogic;
	static std::atomic<bool> g_renderFinished = true;
	static std::atomic<bool> g_logicFinished = false;
	static std::atomic<bool> g_running = true;

	Application::Application(const ApplicationSettings& settings)
		: m_settings(settings) {
		App = this;
	}

	Application::~Application() {
		App = nullptr;
	}

	int x = 0;
	int y = 0;
	void window_refresh_callback(GLFWwindow* window) {
		if (x != 0) {
			FrameBufferManager::onResize(x, y);
			FrameBufferManager::bindDefaultFBO();
			glViewport(0, 0, x, y);
			App->update(Timestep(0, 0, 0));
			x = 0;
			y = 0;
			Renderer::executeCommandBuffer();
			glfwSwapBuffers(window);
		}
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
		m_mainWindow->setVSync(true);
		m_mainWindow->setLimits(200, 60, GLFW_DONT_CARE, GLFW_DONT_CARE);

		//LOG("[~cGPU~x] %-26s %s", "GPU~1", glGetString(GL_RENDERER));
		//LOG("[~cGPU~x] %-26s %s", "OpenGL version~1", glGetString(GL_VERSION));

		imGuiManager::initialize(m_mainWindow);

		onInitialize();
		glfwSetWindowRefreshCallback(m_mainWindow->handle(), window_refresh_callback);

		m_mainWindow->show();

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		GLUtils::setDebugMessageCallback();

		threading::registerThread("Logic", [this]() { logicLoop(); });

		renderLoop();

		close();

		imGuiManager::shutdown();
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
			waitForLogic();

			PROFILE_RENDER_BEGIN("Render");

			PROFILE_RENDER_BEGIN("PollEvents");
			m_mainWindow->pollEvents();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("CommandBuffer");
			Renderer::executeCommandBuffer();
			PROFILE_RENDER_END();

			PROFILE_RENDER_BEGIN("SwapBuffers");
			m_mainWindow->swapBuffers();
			m_fps++;
			PROFILE_RENDER_END();

			g_renderFinished.store(true);
			g_cvLogic.notify_one();

			PROFILE_RENDER_END();
		}
	}


	void Application::logicLoop() {
		PROFILE_REGISTER_LOGIC_THREAD("Logic");

		while (g_running) {
			PROFILE_LOGIC_FRAME();

			waitForRender();

			PROFILE_LOGIC_BEGIN("Logic");
			float currentTime = (float)glfwGetTime();
			float deltaTime = currentTime - m_lastFrameTime;
			m_totalFrameTime += deltaTime;
			m_frameCount++;

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

			PROFILE_LOGIC_BEGIN("Update");
			update(Timestep(deltaTime, m_totalFrameTime, m_frameCount));
			PROFILE_LOGIC_END();

			m_lastFrameTime = currentTime;

			if (currentTime - m_lastTitleUpdateTime >= 1.0f) {
				Log::info("{} | UPS: {} FPS: {}", m_settings.m_name, m_ups, m_fps);

				m_fps = 0;
				m_ups = 0;
				m_lastTitleUpdateTime = currentTime;
			}

			PROFILE_LOGIC_END();
		}

		// Let the render thread know that the logic thread has finished
		g_logicFinished.store(true);
		g_cvRender.notify_one();
	}


	void Application::waitForRender() {
		PROFILE_LOGIC_BEGIN("WaitForRender");
		std::unique_lock<std::mutex> lock(g_mtx);
		g_cvLogic.wait(lock, [] { return g_renderFinished.load(); });
		g_renderFinished.store(false);
		lock.unlock();  // Unlock before notifying to avoid holding the lock while the other thread runs.
		g_logicFinished.store(true);
		g_cvRender.notify_one();
		PROFILE_LOGIC_END();
	}
	void Application::waitForLogic() {
		PROFILE_RENDER_BEGIN("WaitForLogic");
		std::unique_lock<std::mutex> lock(g_mtx);
		g_cvRender.wait(lock, [] { return g_logicFinished.load(); });
		g_logicFinished.store(false);
		lock.unlock();  // Unlock before notifying to avoid holding the lock while the other thread runs.
		g_renderFinished.store(true);
		g_cvLogic.notify_one();
		PROFILE_RENDER_END();
	}

	void Application::close() {
		glfwSetWindowShouldClose(m_mainWindow->handle(), true);
	}

	void Application::onResize(uint32_t width, uint32_t height) {
		x = width;
		y = height;
		//QueueEvent([width, height, this] {
		//	Renderer::submit([width, height, this] {
		//		
		//	});
		//});
		//Renderer::submit([width, height, this] {
		//	FrameBufferManager::onResize(width, height);
		//	glViewport(0, 0, width, height);
		//	update(Timestep(0, m_totalFrameTime, m_frameCount));
		//	m_mainWindow->swapBuffers();
		//});
		//Renderer::executeCommandBuffer();
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