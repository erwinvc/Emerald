#pragma once

class Window;

class ImGuiManager : public Singleton<ImGuiManager> {
private:
    float m_time = 0;
public:
	ImGuiManager() {}
	~ImGuiManager() {}
	friend Singleton;

    void Initialize(Window* window);
    void Begin();
    void End();
};

static ImGuiManager* GetImGuiManager() { return ImGuiManager::GetInstance(); }