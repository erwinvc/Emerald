#pragma once
#include <string>
#include <thread>
#include <functional>
#include <atomic>

namespace emerald {
	enum ThreadType {
		LOGIC,
		RENDER,
		CONSOLE_OUTPUT,
		COUNT
	};

	class Thread {
	public:
		Thread(const std::string& name, std::function<void()> func, bool background = false);
		~Thread();

		void start();
		void shutdown();
		bool isFinished() const { return m_finished; }
		bool isBackground() const { return m_background; }
		const std::string& getName() const { return m_name; }
		const std::thread::id getID() const { return m_handle->get_id(); }

		friend class ThreadManager;

	private:
		std::string m_name;
		std::function<void()> m_function;
		bool m_background;
		std::atomic<bool> m_shutDown;
		std::atomic<bool> m_finished;
		std::unique_ptr<std::thread> m_handle;

		void run();
	};

	class ThreadManager {
	public:
		static Thread* createAndRegisterThread(ThreadType type, const std::string& name, std::function<void()> func, bool background = false);
		static void registerCurrentThread(ThreadType type);
		static bool isThread(ThreadType type);
		static void cleanup();
	};
}
