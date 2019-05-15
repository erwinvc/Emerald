#pragma once
class Application : public Singleton<Application> {
private:
	Application();

	Window* m_window;
	Timer m_timer;
	uint64_t m_frameCount;
	TimeStep m_timeStep;
	RenderingPipeline* m_pipeline;

	bool m_running;
public:
	Window* GetWindow() { return m_window; }

	void Run();
	void Cleanup();
	void OnWindowClose();
	void OnResize(int width, int height);
	void Update(const TimeStep& time);
	void FixedUpdate();
	void Render();

	uint64_t GetFrameCount() { return m_frameCount; }
	RenderingPipeline* GetPipeline() { return m_pipeline; }

	friend Singleton;
};

static Application* GetApplication() {
	return Application::GetInstance();
}

static RenderingPipeline* GetPipeline() {return Application::GetInstance()->GetPipeline(); }
static Camera* GetCamera() { return Application::GetInstance()->GetPipeline()->GetCamera(); }