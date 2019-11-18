#pragma once

struct Fiber {
	void* m_fiber;
	Fiber* m_nextFiber;
	float m_wakeTime;
	String m_name;
	thread::id m_glThreadID;
};

class GLFiberManager : public Singleton<GLFiberManager> {
private:
	GLFiberManager() {}
	~GLFiberManager() {}
	friend Singleton;

	vector<Fiber> m_fibers;

	LPVOID m_mainFiber;
	Fiber* m_currentFiber;

	bool m_initialized = false;
	bool m_cleaned;
	thread::id m_glThreadID;

public:
	void Initialize();
	void Tick();
	void AddFiber(String name, void(*function)());
	void Sleep(float waitTime);
	void GoToMainFiber();
	void Cleanup();
	void Delete(String name);

	vector<Fiber> GetFibers() { return m_fibers; }
	String GetCurrentFiberName() { return m_currentFiber->m_name; }

	bool IsSameThread(thread::id threadId){
		if (!m_initialized) return true;
		return(threadId == m_glThreadID);
	}
};

static GLFiberManager* GetGLFiberManager() { return GLFiberManager::GetInstance(); }