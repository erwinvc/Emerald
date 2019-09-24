#pragma once
class Application : public Singleton<Application> {
private:
	Application() : m_running(true) {}
	~Application() {}
	friend Singleton;

	bool m_initialized = false;
	Window* m_window;
	Timer m_timer;
	uint64_t m_frameCount;
	float m_lastFrameTime;
	int m_fps;

	RenderingPipeline* m_pipeline;
	bool m_running;
	bool m_ImGuiOpen = true;

	HWND m_hwndHandle = 0;

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

	uint64_t GetFrameCount() { return m_frameCount; }
	RenderingPipeline* GetPipeline() { return m_pipeline; }

	inline float GetWidth() { return (float)m_window->GetWidth(); }
	inline float GetHeight() { return (float)m_window->GetHeight(); }
	inline float GetAspect() { return m_window->GetAspect(); }
};

static Application* GetApp() {
	return Application::GetInstance();
}

static RenderingPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
static AssetRef<Camera> GetCamera() { return Application::GetInstance()->GetPipeline()->GetCamera(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }
