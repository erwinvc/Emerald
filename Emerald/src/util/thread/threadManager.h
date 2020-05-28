#pragma once

class Thread {
private:
    String m_name;
    HANDLE m_handle;
    function<void()> m_function;
	atomic<bool> m_shutDown = false;
	atomic<bool> m_finished = false;
	atomic<bool> m_forceKill = false;

	friend class ThreadManager;
public:
    Thread() : m_name(""), m_handle(INVALID_HANDLE_VALUE) {}
    Thread(const String& name) : m_name(name), m_handle(INVALID_HANDLE_VALUE) {}

    HANDLE GetHandle() { return m_handle; };
    void Shutdown() { m_shutDown = true; }
    bool IsShuttingDown() { return m_shutDown; }
    bool IsFinished() { return m_finished; }

    const String& GetName() { return m_name; }
    void SetName(String& name) { m_name = name; }
};

class ThreadManager {
private:
    mutex m_lock;
    vector<Thread*> m_threads;

public:
    Thread* RegisterThread2(String name, function<void()> func, bool forceKill = false);
    Thread* RegisterThread(String name, void(*func)(), bool forceKill = false);
    void Cleanup();
};

ThreadManager* GetThreadManager();