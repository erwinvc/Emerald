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

	void AddFiber(String name, void(*function)()) {
		UINT64* args = new UINT64[2]{ (UINT64)function, (UINT64)m_mainFiber };
		LPVOID fiber = CreateFiber(NULL, [](LPVOID lpFiberParameter) {
			UINT64* arguments = (UINT64*)lpFiberParameter;
			try {
				while (true) {
					((void(*)(void)) arguments[0])();
					SwitchToFiber((LPVOID)arguments[1]);
				}
			} catch (...) {
				LOG_ERROR("[~rThreads~x] caught exception in fiber");
				delete[] arguments;
			}
		}, args);

		Fiber newFiber = { fiber, nullptr, (float)glfwGetTime(), name };
		m_fibers.push_back(newFiber);

		for (auto& fiber = m_fibers.begin(); fiber != m_fibers.end(); ++fiber) {
			auto& nextFiber = fiber + 1;
			if (nextFiber != m_fibers.end()) fiber->m_nextFiber = &*nextFiber;
			else fiber->m_nextFiber = &m_fibers.front();
		}
		m_currentFiber = &m_fibers.front();
		LOG("[~rThreads~x] Created ~1%s~x fiber", newFiber.m_name.c_str());
	}

	void Sleep(float waitTime);
	void GoToMainFiber();
	void Cleanup();
	void Delete(String name);

	vector<Fiber> GetFibers() { return m_fibers; }
	String GetCurrentFiberName() { return m_currentFiber->m_name; }

	bool IsSameThread(thread::id threadId)
	{
		if (!m_initialized) return true;
		return(threadId == m_glThreadID);
	}
};

static GLFiberManager* GetGLFiberManager() { return GLFiberManager::GetInstance(); }