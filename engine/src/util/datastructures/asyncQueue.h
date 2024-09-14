#pragma once
#include <mutex>
#include <queue>
#include <stdexcept>

namespace emerald {
	// Thread-safe queue for asynchronous inter-thread communication.
	template<typename T>
	class AsyncQueue {
	private:
		mutable std::mutex m_lock;
		std::condition_variable m_conditionVariable;
		std::queue <T> m_queue;
		bool m_releaseThreads = false;

	public:
		AsyncQueue() : m_releaseThreads(false) {}
		AsyncQueue(const AsyncQueue&) = delete;
		AsyncQueue& operator=(const AsyncQueue&) = delete;

		void add(T obj) {
			std::lock_guard<std::mutex> l(m_lock);
			if (m_releaseThreads) {
				throw std::runtime_error("Attempted to add to a released queue");
			}
			m_queue.emplace(std::move(obj));
			m_conditionVariable.notify_one();
		}

		bool tryToGet(T& obj) {
			std::lock_guard<std::mutex> lock(m_lock);
			if (m_queue.empty()) return false;
			obj = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}

		bool peek(T& obj) {
			std::lock_guard<std::mutex> lock(m_lock);
			if (m_queue.empty()) return false;
			obj = m_queue.front();
			return true;
		}

		bool waitForGet(T& obj) {
			std::unique_lock<std::mutex> lock(m_lock);
			while (!m_releaseThreads && m_queue.empty()) m_conditionVariable.wait(lock);
			if (m_queue.empty()) return false;
			obj = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}

		void shutdown() {
			{
				std::lock_guard<std::mutex> lock(m_lock);
				m_releaseThreads = true;
			}
			m_conditionVariable.notify_all();
		}

		bool isShutdown() const {
			std::lock_guard<std::mutex> lock(m_lock);
			return m_releaseThreads;
		}

		size_t size() const {
			std::lock_guard<std::mutex> lock(m_lock);
			return m_queue.size();
		}

		bool empty() const {
			std::lock_guard<std::mutex> lock(m_lock);
			return m_queue.empty();
		}
	};
}