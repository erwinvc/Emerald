#pragma once
#include "utils/datastructures/asyncQueue.h"
#include <functional>
#include <future>
#include <vector>
#include "threadManager.h"

namespace emerald {
	class JobSystem {
	public:
		JobSystem() = delete;
		JobSystem(const JobSystem&) = delete;
		JobSystem& operator=(const JobSystem&) = delete;

		static void initialize() {
			size_t threadCount = std::max(1u, std::thread::hardware_concurrency());
			if (!s_workerThreads.empty()) {
				return;
			}

			s_isRunning = true;
			s_workerThreads.reserve(threadCount);

			for (size_t i = 0; i < threadCount; ++i) {
				Thread* thread = ThreadManager::createAndRegisterThread(ThreadType::LOGIC, std::format("Worker_{}", i), [i]() { workerLoop(i); });
				s_workerThreads.emplace_back(thread);
			}
		}

		// Static job submission methods
		template<typename Func, typename... Args>
		static auto submit(Func&& func, Args&&... args) -> std::future<std::invoke_result_t<Func, Args...>> {
			using ReturnType = std::invoke_result_t<Func, Args...>;

			auto task = std::make_shared<std::packaged_task<ReturnType()>>(
				std::bind(std::forward<Func>(func), std::forward<Args>(args)...)
			);

			auto future = task->get_future();

			s_jobQueue.add([task]() {
				(*task)();
			});

			s_jobCondVar.notify_one();
			return future;
		}

		static void submit(std::function<void()> job) {
			s_jobQueue.add(std::move(job));
			s_jobCondVar.notify_one();
		}

		static void shutdown() {
			s_isRunning = false;
			s_jobCondVar.notify_all();

			for (auto& thread : s_workerThreads) {
				thread->shutdown();
			}
			s_workerThreads.clear();
		}

		static size_t getThreadCount() {
			return s_workerThreads.size();
		}

	private:
		static void workerLoop(size_t threadIndex) {
			while (s_isRunning) {
				std::function<void()> job;

				{
					std::unique_lock<std::mutex> lock(s_jobMutex);
					s_jobCondVar.wait(lock, []() {
						return !s_isRunning || !s_jobQueue.empty();
					});

					if (!s_isRunning && s_jobQueue.empty()) {
						return;
					}
				}

				if (s_jobQueue.tryGet(job)) {
					job();
				}
			}
		}

		static inline AsyncQueue<std::function<void()>> s_jobQueue;
		static inline std::vector<Thread*> s_workerThreads;
		static inline std::mutex s_jobMutex;
		static inline std::condition_variable s_jobCondVar;
		static inline std::atomic<bool> s_isRunning{ false };
	};
}