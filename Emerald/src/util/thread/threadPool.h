#pragma once

class ThreadPool : public Singleton<ThreadPool>{
private:
	bool m_initialized = false;
	AsyncQueue<function <void(void)>> m_queue;

	void ThreadEntry();

public:
	void Initialize(int threads);
	void DoJob(function <void(void)> func);
	bool IsInitialized() { return m_initialized; }
	void Shutdown() { m_queue.ReleaseWaitingThreads(); }
};

static ThreadPool* GetThreadPool() { return ThreadPool::GetInstance(); }