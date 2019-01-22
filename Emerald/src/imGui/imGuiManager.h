#pragma once

class Window;

class ImGuiManager {
private:
    static ImGuiManager* g_instance;
    float m_time;
public:
    static ImGuiManager* GetInstance() {
        if (!g_instance) g_instance = new ImGuiManager();
        return g_instance;
    }

    ImGuiManager() : m_time(0) {}
    void Initialize(Window* window);
    void Begin();
    void End();
};

static ImGuiManager* GetImGuiManager() { return ImGuiManager::GetInstance(); }