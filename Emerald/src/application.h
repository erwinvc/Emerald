#pragma once
class Application : public Singleton<Application> {
private:
	Application() : m_running(true) {}
	~Application() {}
	friend Singleton;

	bool m_initialized = false;
	Window* m_window = nullptr;
	Timer m_timer;
	
	float m_totalFrameTime = 0;
	uint64 m_frameCount = 0;
	float m_lastFrameTime = 0;
	int m_fps = 0;

	bool m_running = true;

	HWND m_hwndHandle = 0;
	AsyncQueue<function<void()>> m_queue;

	void HandleQueue();

public:
	//static uint s_globalLogValue;
	HDRPipeline* pipeline = nullptr;
	
	float GetTotalFrameTime() { return m_totalFrameTime; }
	Window* GetWindow() { return m_window; }

	void Initialize();
	void Run();
	void Cleanup();
	void OnWindowClose();

	void OnResize(int width, int height);
	void Update(TimeStep time);
	void Render();

	void CapabilitiesCheck();
	void QueueTask(function<void()> task) {
		m_queue.Add(task);
	}
	uint64_t GetFrameCount() { return m_frameCount; }

	uint GetWidth() { return m_window->GetWidth(); }
	uint GetHeight() { return m_window->GetHeight(); }
	
	float GetAspect() { return m_window->GetAspect(); }
};

static Application* GetApp() {
	return Application::GetInstance();
}

//static GraphicsPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }