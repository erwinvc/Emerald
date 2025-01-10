#pragma once
#include <mutex>
#include <queue>
#include <stdexcept>

namespace emerald {
	// Thread-safe queue for asynchronous inter-thread communication.
	template<typename T>
	class AsyncQueue {
	public:
		class QueueShutdownException : public std::runtime_error {
		public:
			QueueShutdownException() : std::runtime_error("AsyncQueue already released") {}
		};

		AsyncQueue() = default;
		AsyncQueue(const AsyncQueue&) = delete;
		AsyncQueue& operator=(const AsyncQueue&) = delete;
		AsyncQueue(AsyncQueue&&) = delete;
		AsyncQueue& operator=(AsyncQueue&&) = delete;

		// Moves the item into the queue
		template<typename U>
		void emplace(U&& obj) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_released) {
				throw QueueShutdownException();
			}
			m_queue.emplace(std::forward<U>(obj));
			m_condVar.notify_one();
		}

		// Copies the item into the queue
		template <typename U>
		void pushBack(const U& item) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_released) {
				throw QueueShutdownException();
			}
			m_queue.push_back(item);
			m_condVar.notify_one();
		}

		template<typename InputIt>
		void emplaceBatch(InputIt first, InputIt last) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_released) {
				throw QueueShutdownException();
			}
			for (; first != last; ++first) {
				m_queue.emplace(*first);
			}
			m_condVar.notify_all();
		}

		bool tryGet(T& obj) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_queue.empty()) return false;
			obj = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}

		std::optional<T> waitAndGet() {
			std::lock_guard<std::mutex> lock(m_mutex);

			m_condVar.wait(lock, [this] {
				return m_released || !m_queue.empty();
				});

			if (m_queue.empty()) {
				return std::nullopt;
			}

			T value = std::move(m_queue.front());
			m_queue.pop();
			return value;
		}

		bool peek(T& obj) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_queue.empty()) return false;
			obj = m_queue.front();
			return true;
		}

		bool waitForGet(T& obj) {
			std::unique_lock<std::mutex> lock(m_mutex);

			while (!m_released && m_queue.empty()) m_condVar.wait(lock);
			if (m_queue.empty()) return false;
			obj = std::move(m_queue.front());
			m_queue.pop();
			return true;
		}

		void swap(AsyncQueue& other) noexcept {
			if (this == &other) return;

			std::scoped_lock lock(m_mutex, other.m_mutex);

			m_queue.swap(other.m_queue);
			std::swap(m_released, other.m_released);
		}

		friend void swap(AsyncQueue& lhs, AsyncQueue& rhs) noexcept {
			lhs.swap(rhs);
		}

		void clear() {
			std::lock_guard<std::mutex> lock(m_mutex);

			std::queue<T>().swap(m_queue);
		}

		void release() {
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				m_released = true;
			}
			m_condVar.notify_all();
		}

		bool empty() const noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_queue.empty();
		}

		size_t size() const noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_queue.size();
		}

		bool isReleased() const noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);

			return m_released;
		}

	private:
		mutable std::mutex m_mutex;
		std::condition_variable m_condVar;
		std::queue <T> m_queue;
		bool m_released = false;

	};
}