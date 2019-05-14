#include "stdafx.h"

ThreadManager g_threadManager;

Thread* ThreadManager::RegisterThread(String name, void(*func)()) {
	Thread* c_thread = new Thread(name);
	UINT64* args = new UINT64[3]{ 0, (UINT64)c_thread, (UINT64)func };

	c_thread->m_handle = CreateThread(0, 0, [](LPVOID lpFiberParameter) -> DWORD {
		UINT64* arguments = (UINT64*)lpFiberParameter;
		try {
			while (!(*(Thread*)arguments[1]).m_shutDown) {
				((void(*)(void)) arguments[2])();
			}
			LOG("[~rThreads~x] %s thread finished", (*(Thread*)arguments[1]).m_name.c_str());
			(*(Thread*)arguments[1]).m_finished = true;
		} catch (...) {
			delete[] arguments;
		}
		return 1;
	}, args, 0, 0);

	unique_lock<mutex> lock(m_lock);
	m_threads.push_back(c_thread);

	LOG("[~rThreads~x] %s thread registered", name.c_str());
	return c_thread;
}

void ThreadManager::Cleanup() {
	for (auto& c_thread : m_threads) {
		c_thread->Shutdown();
	}
	m_conditionVariable.notify_all();

	LOG("[~rThreads~x] Thread pool terminated");
}

ThreadManager* GetThreadManager() { return &g_threadManager; }