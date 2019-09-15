#include "stdafx.h"

static ThreadPool g_pool;

void ThreadPool::Initialize(int threads) {
	if (m_initialized) return;
	m_initialized = true;

	for (int i = 0; i < threads; ++i) {
		GetThreadManager()->RegisterThread(Format("Job thread %d", i), [] {GetThreadPool()->ThreadEntry(); });
	}
	LOG("[~rThreads~x] Thread pool created");
}

void ThreadPool::DoJob(std::function <void(void)> func) {
	if (!m_initialized) {
		LOG_ERROR("[~rThreads~x] Thread pool has not yet been initialized");
		return;
	}
	std::unique_lock <std::mutex> l(m_lock);
	m_jobs.emplace(std::move(func));
	m_conditionVariable.notify_one();
}

void ThreadPool::ThreadEntry() {
	function <void(void)> job;
	while (1) {
		{
			std::unique_lock <mutex> lock(m_lock);
			while (!m_shutdown && m_jobs.empty()) m_conditionVariable.wait(lock);
			if (m_jobs.empty())return;
			job = std::move(m_jobs.front());
			m_jobs.pop();
		}
		job();
	}
}