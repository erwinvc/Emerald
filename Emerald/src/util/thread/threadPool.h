#pragma once

class ThreadPool : public Singleton<ThreadPool>{
public:
	void Initialize(int threads);
	void DoJob(std::function <void(void)> func);
	bool IsInitialized() { return m_initialized; }
	void Shutdown() { m_shutdown = true; }

private:
	bool m_initialized = false;
	bool m_shutdown = false;
	mutex m_lock;
	condition_variable m_conditionVariable;
	queue <std::function <void(void)>> m_jobs;

	void ThreadEntry();
};

static ThreadPool* GetThreadPool() { return ThreadPool::GetInstance(); }