#pragma once
class Application {
private:
    Window* m_window;
    Timer* m_timer;
    TimeStep* m_timeStep;
    bool m_running;
public:
    Application();
    ~Application();
    void Application::OnEvent(Event& e);
    void Run();
    void Cleanup();
    bool OnWindowClose();

    void Update(const TimeStep& time);
    void FixedUpdate();
    void Render();
};

