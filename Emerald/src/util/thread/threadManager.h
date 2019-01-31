#pragma once

class Thread {
private:
    String m_name;
    HANDLE m_thread;
    atomic<bool> m_shutDown;
public:
    Thread() : m_name(""), m_thread(INVALID_HANDLE_VALUE), m_shutDown(false) {}
    Thread(String name) : m_name(name), m_thread(INVALID_HANDLE_VALUE), m_shutDown(false) {}

    void SetHandle(HANDLE handle) { m_thread = handle; }
    HANDLE GetHandle() { return m_thread; };
    void Shutdown() { m_shutDown = true; }
    bool IsShutdown() { return m_shutDown; }

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