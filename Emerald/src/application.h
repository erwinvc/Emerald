#pragma once
class Application : public Singleton<Application> {
private:
	Application() : m_running(true) {}
	~Application() {}
	friend Singleton;

	bool m_initialized = false;
	Window* m_window = nullptr;
	Timer m_timer;
	uint64_t m_frameCount = 0;
	float m_lastFrameTime = 0;
	int m_fps = 0;

	RenderingPipeline* m_pipeline = nullptr;
	bool m_running = true;
	bool m_ImGuiOpen = true;

	HWND m_hwndHandle = 0;
	AsyncQueue<function<void()>> m_queue;

	void HandleQueue();

public:
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
	RenderingPipeline* GetPipeline() { return m_pipeline; }

	template<typename T>
	inline T GetWidth() { return m_window->GetWidth<T>(); }

	template<typename T>
	inline T GetHeight() { return m_window->GetHeight<T>(); }
	inline float GetAspect() { return m_window->GetAspect(); }

};

static Application* GetApp() {
	return Application::GetInstance();
}

static RenderingPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
static Camera* GetCamera() { return Application::GetInstance()->GetPipeline()->GetCamera(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }