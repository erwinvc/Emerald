#pragma once

static void ErrorCallback(int error, const char* description) {
	LOG_ERROR("[GLFW] %s", description);
}

static void DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	LOG("%d %d %s", type, id, message);
}

struct temp
{
	bool a;
};
class Application : public Singleton<Application> {
private:
	bool m_isServer;

	String GetTitle() {
		return m_isServer ? "EmeraldServer" : "Emerald";
	}

	void Cleanup();
	
public:
	void Initialize(bool isServer);
	inline bool IsServer() { return m_isServer; }
};

inline Application* GetApp() { return Application::GetInstance(); }