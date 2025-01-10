#pragma once
#include "engine/events/events.h"
#include "graphics/textures/texture.h"
#include "utils/datastructures/asyncQueue.h"
#include "utils/system/timestep.h"
#include <functional>
#include <string>

namespace emerald {
	class Window;
	class Texture;
	class CommandBuffer;

	struct ApplicationSettings {
		std::string m_name;
		uint32_t m_width, m_height;
	};

	struct ResizeData {
		uint32_t m_width, m_height;
		bool m_shouldResize;
	};

	class Application {
	public:
		Application(const ApplicationSettings& settings = { "Emerald", 1920, 1080 });
		virtual ~Application();
		void run();
		void close();
		void handleResize();
		void onResize(uint32_t width, uint32_t height);
		
		virtual void onInitialize() = 0;
		virtual void onShutdown() = 0;
		virtual void update(Timestep ts) = 0;
		virtual void fixedUpdate(Timestep ts) = 0;
		virtual void onEvent(Event& event) = 0;

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		float getTime() const;
		const Ref<Window>& getWindow() { return m_mainWindow; }
		bool isFocused() const;

		template<typename F>
		void QueueEvent(F&& function) {
			m_eventQueue.emplace(function);
		}
		template<typename F>
		void QueueEventCPU(F&& function) {
			m_eventQueueCPU.emplace(function);
		}
		void processQueue();
		void processQueueCPU();

		uint64_t getFrameCount() const { return m_frameCount; }
		uint32_t getFPS() const { return m_fps; }
		uint32_t getUPS() const { return m_ups; }
		float getTotalFrameTime() const { return m_totalFrameTime; }
		float getLastFrameTime() const { return m_lastFrameTime; }
		float getFixedTimeStep() const { return m_fixedTimeStep; }
		void setFixedTimeStep(float timeStep) { m_fixedTimeStep = timeStep; }

	private:
		void initializeLogic();
		void logicLoop();
		void renderLoop();

	protected:
		ApplicationSettings m_settings;
		Ref<Window> m_mainWindow;
		AsyncQueue<std::function<void()>> m_eventQueue;
		AsyncQueue<std::function<void()>> m_eventQueueCPU;

		uint64_t m_frameCount = 0;
		uint32_t m_fpsCounter = 0;
		uint32_t m_upsCounter = 0;
		uint32_t m_fps = 0;
		uint32_t m_ups = 0;
		float m_fixedTimeStep = 0.02f; // Fixed timestep of 20 ms (50 times per second)
		float m_accumulatedTime = 0.0f;
		float m_totalFrameTime = 0.0f;
		float m_lastFrameTime = 0.0f;
		float m_upsfpsCounter = 0.0f;
	};

	inline Application* App;
	Application* createApplication();
}