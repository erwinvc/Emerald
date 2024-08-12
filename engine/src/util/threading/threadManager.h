#pragma once
#include <string>
#include <thread>
#include <functional>
#include <atomic>

namespace emerald::threading {
	class Thread {
	public:
		Thread(const std::string& name, std::function<void()> func, bool background = false);
		~Thread();

		void start();
		void shutdown();
		bool isFinished() const { return m_finished; }
		bool isBackground() const { return m_background; }
		const std::string& getName() const { return m_name; }

		friend class ThreadManager;

	private:
		std::string m_name;
		std::function<void()> m_function;
		bool m_background;
		std::atomic<bool> m_shutDown;
		std::atomic<bool> m_finished;
		std::unique_ptr<std::thread> m_thread;

		void run();
	};

	Thread* registerThread(const std::string& name, std::function<void()> func, bool background = false);
	void cleanup();
}
