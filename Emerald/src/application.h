#pragma once
class Application {
private:
private:
    static Application* g_instance;
    Application();

    Window* m_window;
    Timer* m_timer;
    TimeStep* m_timeStep;
    bool m_running;
public:
    static Application* GetInstance() {
        if (!g_instance) g_instance = new Application();
        return g_instance;
    }

    Window* GetWindow() { return m_window; }

    void OnEvent(Event& e);
    void Run();
    void Cleanup();
    bool OnWindowClose();

    void Update(const TimeStep& time);
    void FixedUpdate();
    void Render();
};

static Application* GetApplication() {
    return Application::GetInstance();
}
