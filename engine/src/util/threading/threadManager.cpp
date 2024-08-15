#include "eepch.h"
#include "threadManager.h"
#include <mutex>
#include <vector>
#include <condition_variable>

namespace emerald::threading {
	std::vector<std::unique_ptr<Thread>> m_threads;
	std::mutex m_lock;
	std::condition_variable m_condVar;

	Thread::Thread(const std::string& name, std::function<void()> func, bool background)
		: m_name(name), m_function(std::move(func)), m_background(background), m_shutDown(false), m_finished(false) {
	}

	Thread::~Thread() {
		if (m_handle && m_handle->joinable()) {
			m_handle->join();
		}
	}

	void Thread::start() {
		std::wstring wsTmp(m_name.begin(), m_name.end());
		m_handle = std::make_unique<std::thread>(&Thread::run, this);
		SetThreadDescription(m_handle->native_handle(), wsTmp.c_str());
		if (m_background) {
			m_handle->detach();
		}
	}

	void Thread::shutdown() {
		if (m_shutDown) return;
		m_shutDown = true;
		if (m_handle && m_handle->joinable()) {
			m_handle->join();
		}
	}

	void Thread::run() {
		//try {
			m_function();
			//} catch (const std::exception& e) {
			//	Log::fatal("Exception in thread {}: {}", m_name.c_str(), e.what());
			//} catch (...) {
			//	Log::fatal("Unknown exception in thread {}", m_name.c_str());
			//}
		m_finished = true;
	}

	Thread* registerThread(const std::string& name, std::function<void()> func, bool background) {
		std::lock_guard<std::mutex> lock(m_lock);
		m_threads.emplace_back(std::make_unique<Thread>(name, func, background));
		Thread* newThread = m_threads.back().get();
		newThread->start();
		return newThread;
	}

	void cleanup() {
		for (auto& thread : m_threads) {
			if (!thread->isFinished()) {
				thread->shutdown();
			}
		}

		// Wait for non-background threads to finish
		std::unique_lock<std::mutex> lock(m_lock);
		m_condVar.wait(lock, [] {
			for (auto& thread : m_threads) {
				if (!thread->isFinished() && !thread->isBackground()) {
					return false;
				}
			}
			return true;
		});
	}
}
