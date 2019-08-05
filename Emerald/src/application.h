#pragma once
class Application : public Singleton<Application> {
private:
	Application() : m_running(true) {}
	~Application() {}
	friend Singleton;

	Window* m_window;
	Timer m_timer;
	uint64_t m_frameCount;
	float m_lastFrameTime;
	int m_fps;

	RenderingPipeline* m_pipeline;
	bool m_running;
	bool m_ImGuiOpen = true;
public:
	Window* GetWindow() { return m_window; }

	void Initialize();
	void Run();
	void Cleanup();
	void OnWindowClose();

	void OnResize(int width, int height);
	void Update(TimeStep time);
	void Render();

	uint64_t GetFrameCount() { return m_frameCount; }
	RenderingPipeline* GetPipeline() { return m_pipeline; }

	inline int GetWidth() { return m_window->GetWidth(); }
	inline int GetHeight() { return m_window->GetHeight(); }
};

static Application* GetApplication() {
	return Application::GetInstance();
}

static RenderingPipeline* GetPipeline() { return Application::GetInstance()->GetPipeline(); }
static Camera* GetCamera() { return Application::GetInstance()->GetPipeline()->GetCamera(); }
//static GBuffer* GetGBuffer() { return Application::GetInstance()->GetPipeline()->GetGBuffer(); }
