#pragma once

class Window;

class ImGuiManager : public Singleton<ImGuiManager> {
private:
	bool m_initialized = false;
    float m_time = 0;
public:
	ImGuiManager() {}
	~ImGuiManager() {}
	friend Singleton;

	bool IsInitialized() { return m_initialized; }
    void Initialize(Window* window);
    void Begin();
    void End();
};

static ImGuiManager* GetImGuiManager() { return ImGuiManager::GetInstance(); }