#pragma once
#include <stack>

namespace emerald {
	// Thread-safe stack for asynchronous inter-thread communication.
	template<typename T>
	class AsyncStack {
	public:
		class StackShutdownException : public std::runtime_error {
		public:
			StackShutdownException() : std::runtime_error("AsyncStack already released") {}
		};

		AsyncStack() = default;
		AsyncStack(const AsyncStack&) = delete;
		AsyncStack& operator=(const AsyncStack&) = delete;
		AsyncStack(AsyncStack&&) = delete;
		AsyncStack& operator=(AsyncStack&&) = delete;

		template<typename U>
		void push(U&& obj) {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_released) {
				throw StackShutdownException();
			}
			m_stack.push(std::forward<U>(obj));
			m_condVar.notify_one();
		}

		template<typename InputIt>
		void pushBatch(InputIt first, InputIt last) {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_released) {
				throw StackShutdownException();
			}
			for (; first != last; ++first) {
				m_stack.push(*first);
			}
			m_condVar.notify_all();
		}

		bool pop(T& obj) noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_stack.empty()) {
				return false;
			}
			obj = std::move(m_stack.top());
			m_stack.pop();
			return true;
		}

		bool pop() noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_stack.empty()) {
				return false;
			}
			m_stack.pop();
			return true;
		}

		std::optional<T> waitAndPop() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condVar.wait(lock, [this] {
				return m_released || !m_stack.empty();
			});

			if (m_stack.empty()) {
				return std::nullopt;
			}

			T value = std::move(m_stack.top());
			m_stack.pop();
			return value;
		}

		bool peek(T& obj) {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_stack.empty()) return false;
			obj = m_stack.top();
			return true;
		}

		void clear() noexcept {
			std::lock_guard<std::mutex> lock(m_mutex);
			std::stack<T>().swap(m_stack); 
		}

		void release() {
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				m_released = true;
			}
			m_condVar.notify_all();
		}

		bool empty() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_stack.empty();
		}

		size_t size() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_stack.size();
		}

		bool isReleased() {
			std::lock_guard<std::mutex> lock(m_mutex);
			return m_released;
		}

		void reverse() {
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_stack.empty()) return;

			std::vector<T> temp;
			temp.reserve(m_stack.size());

			while (!m_stack.empty()) {
				temp.push_back(std::move(m_stack.top()));
				m_stack.pop();
			}

			for (auto& item : temp) {
				m_stack.push(std::move(item));
			}
		}

		std::vector<T> drain() {
			std::lock_guard<std::mutex> lock(m_mutex);
			std::vector<T> result;
			result.reserve(m_stack.size());

			while (!m_stack.empty()) {
				result.push_back(std::move(m_stack.top()));
				m_stack.pop();
			}

			return result;
		}

	private:
		mutable std::mutex m_mutex;
		std::condition_variable m_condVar;
		std::stack<T> m_stack;
		bool m_released = false;
	};
}