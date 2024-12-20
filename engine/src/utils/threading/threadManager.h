#pragma once
#include <string>
#include <thread>
#include <functional>
#include <atomic>
#include <future>
#include "../datastructures/asyncQueue.h"

namespace emerald {
	enum ThreadType {
		LOGIC,
		ASSETLOADING,
		RENDER,
		CONSOLE_OUTPUT,
		_COUNT
	};

	class Thread {
	public:
		Thread(const std::string& name, std::function<void()> func, uint32_t affinity, bool background = false);
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
		uint32_t m_affinity;
		bool m_background;
		std::atomic<bool> m_shutDown;
		std::atomic<bool> m_finished;
		std::unique_ptr<std::thread> m_handle;

		void run();
		void setAffinity();
	};

	class ThreadManager {
	public:
		ThreadManager() = delete;
		ThreadManager(const ThreadManager&) = delete;
		ThreadManager& operator=(const ThreadManager&) = delete;

		static Thread* createAndRegisterThread(ThreadType type, const std::string& name, std::function<void()> func, bool background = false);
		static void registerCurrentThread(ThreadType type);
		static bool isThread(ThreadType type);
		static void shutdown();

	private:
		static inline std::atomic<uint32_t> s_nextAffinity{ 0 };
		static inline uint32_t s_maxAffinity;

		static void initializeAffinitySystem();
	};
}
