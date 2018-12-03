#pragma once
class Application {
private:
	Window* m_window;
public:
	Application();
	~Application();
	void Run();
	void Cleanup();
};

