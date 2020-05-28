#include "stdafx.h"

ThreadManager g_threadManager;

Thread* ThreadManager::RegisterThread2(String name, function<void()> func, bool forceKill) {
	Thread* c_thread = NEW(Thread(name));
	c_thread->m_forceKill = forceKill;
	c_thread->m_function = func;
	uint64* args = new uint64[2]{ 0, (uint64)c_thread };
	c_thread->m_handle = CreateThread(0, 0, [](void* lpFiberParameter) -> DWORD {
		uint64* arguments = (uint64*)lpFiberParameter;
		Thread* thread = (Thread*)arguments[1];
		//try {
		while (!thread->m_shutDown) {
			thread->m_function;
		}
		LOG("[~rThreads~x] shut down ~1%s~x thread", thread->GetName().c_str());
		thread->m_finished = true;

		//} catch (...) { LOG_ERROR("[~rThreads~x] caught exception in ~1%s ~rthread", thread->m_name.c_str()); }
		DELETE(thread);
		DELETEARRAY(arguments);
		return 1;
	}, args, 0, 0);

	unique_lock<mutex> lock(m_lock);
	m_threads.push_back(c_thread);

	LOG("[~rThreads~x] registered ~1%s ~xthread ", name.c_str());
	return c_thread;
}

Thread* ThreadManager::RegisterThread(String name, void(*func)(), bool forceKill) {
	Thread* c_thread = NEW(Thread(name));
	c_thread->m_forceKill = forceKill;
	uint64* args = new uint64[3]{ 0, (uint64)c_thread, (uint64)func };
	c_thread->m_handle = CreateThread(0, 0, [](void* lpFiberParameter) -> DWORD {
		uint64* arguments = (uint64*)lpFiberParameter;
		Thread* thread = (Thread*)arguments[1];
		//try {
		while (!thread->m_shutDown) {
			((void(*)(void)) arguments[2])();
		}
		LOG("[~rThreads~x] shut down ~1%s~x thread", thread->GetName().c_str());
		thread->m_finished = true;

		//} catch (...) { LOG_ERROR("[~rThreads~x] caught exception in ~1%s ~rthread", thread->m_name.c_str()); }
		DELETE(thread);
		DELETEARRAY(arguments);
		return 1;
	}, args, 0, 0);

	unique_lock<mutex> lock(m_lock);
	m_threads.push_back(c_thread);

	LOG("[~rThreads~x] registered ~1%s ~xthread ", name.c_str());
	return c_thread;
}

void ThreadManager::Cleanup() {
	GetThreadPool()->Shutdown();
	for (Thread* thread : m_threads) {
		thread->Shutdown();
		if (thread->m_forceKill) {
			TerminateThread(thread->m_handle, 0);
			thread->m_finished = true;
		}
	}
	bool finished = false;
	do {
		finished = true;
		for (auto& thread : m_threads) {
			if (!thread->IsFinished()) finished = false;
		}
		Sleep(0);
	} while (!finished);
	LOG("[~rThreads~x] Thread pool terminated");
}

ThreadManager* GetThreadManager() { return &g_threadManager; }