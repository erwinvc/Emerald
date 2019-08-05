#include "stdafx.h"

ThreadManager g_threadManager;

Thread* ThreadManager::RegisterThread(String name, void(*func)()) {
	Thread* c_thread = NEW(Thread(name));
	uint64* args = new uint64[3]{ 0, (uint64)c_thread, (uint64)func };

	c_thread->m_handle = CreateThread(0, 0, [](void* lpFiberParameter) -> DWORD {
		uint64* arguments = (uint64*)lpFiberParameter;
		Thread* thread = (Thread*)arguments[1];
		try {
			while (!thread->m_shutDown) {
				((void(*)(void)) arguments[2])();
			}
			LOG("[~rThreads~x] ~1%s~xthread finished", thread->GetName().c_str());
			thread->m_finished = true;

		} catch (...) { LOG_ERROR("[~rThreads~x] caught exception in ~1%s ~rthread"); }
		DELETE(thread);
		DELETEARRAY(arguments);
		return 1;
	}, args, 0, 0);

	unique_lock<mutex> lock(m_lock);
	m_threads.push_back(c_thread);

	LOG("[~rThreads~x] ~1%s ~xthread registered", name.c_str());
	return c_thread;
}

void ThreadManager::Cleanup() {
	for (Thread* thread : m_threads) {
		thread->Shutdown();
	}
	//LOG("[~rThreads~x] Thread pool terminated");
}

ThreadManager* GetThreadManager() { return &g_threadManager; }