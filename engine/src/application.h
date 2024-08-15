#pragma once
#include "util/timestep.h"
#include <string>
#include <functional>
#include "util/datastructures/asyncQueue.h"
#include "graphics/texture.h"

namespace emerald {
	class Window;
	class Texture;

	struct ApplicationSettings {
		std::string m_name;
		uint32_t m_width, m_height;
	};

	class Application {
	public:
		Application(const ApplicationSettings& settings = { "Emerald", 1920, 1080 });
		virtual ~Application();

		void run();
		void close();
		void onResize(uint32_t width, uint32_t height);

		virtual void onInitialize() = 0;
		virtual void onShutdown() = 0;
		virtual void update(Timestep ts) = 0;
		virtual void fixedUpdate(Timestep ts) = 0;

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		double getTime() const;
		Ref<Window>& getWindow() { return m_mainWindow; }

		template<typename F>
		void QueueEvent(F&& function) {
			m_eventQueue.add(function);
		}

	private:
		void logicLoop();
		void renderLoop();
		void waitForLogic();
		void waitForRender();

	private:
		ApplicationSettings m_settings;
		Ref<Window> m_mainWindow;
		AsyncQueue<std::function<void()>> m_eventQueue;

		const float m_fixedTimeStep = 0.02f; // Fixed timestep of 20 ms (50 times per second)
		float accumulatedTime = 0.0f;
		float m_totalFrameTime = 0.0f;
		float m_lastFrameTime = 0.0f;
		float m_lastTitleUpdateTime = 0.0f;
		uint64_t m_frameCount = 0;
		uint32_t m_fps = 0;
		uint32_t m_ups = 0;
	};

	inline Application* App;
	Application* createApplication(int argc, char** argv);
}