#pragma once

class Thread {
private:
    String m_name;
    HANDLE m_handle;
	atomic<bool> m_shutDown = false;
	atomic<bool> m_finished = false;

	friend class ThreadManager;
public:
    Thread() : m_name(""), m_handle(INVALID_HANDLE_VALUE) {}
    Thread(String name) : m_name(name), m_handle(INVALID_HANDLE_VALUE) {}

    HANDLE GetHandle() { return m_handle; };
    void Shutdown() { m_shutDown = true; }
    bool IsFinished() { return m_finished; }

    String GetName() { return m_name; }
    void SetName(String& name) { m_name = name; }
};

class ThreadManager {
private:
    condition_variable m_conditionVariable;
    mutex m_lock;
    vector<Thread*> m_threads;

public:
    Thread* RegisterThread(String name, void(*func)());
    void Cleanup();
};

ThreadManager* GetThreadManager();