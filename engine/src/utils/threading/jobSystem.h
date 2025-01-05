#pragma once
#include "utils/datastructures/asyncQueue.h"
#include <functional>
#include <future>
#include <vector>
#include "threadManager.h"

namespace emerald {
	enum class Priority
	{
		HIGH,
		LOW,
		_COUNT
	};

	class Context
	{
	public:
		bool isBusy() const;
		void wait();

	private:
		std::atomic<uint32_t> counter{ 0 };
		Priority priority = Priority::HIGH;
		friend class JobSystem;
	};

	struct JobArgs
	{
		uint32_t m_jobIndex = ~0;
		uint32_t m_groupID = ~0;
		uint32_t m_groupIndex = ~0;
	};

	struct Job {
		std::function<void(JobArgs)> m_function = nullptr;
		Context* m_ctx = nullptr;
		uint32_t m_groupID = ~0;
		uint32_t m_groupJobOffset = ~0;
		uint32_t m_groupJobEnd = ~0;
	};

	class JobSystem {
	public:
		JobSystem() = delete;
		JobSystem(const JobSystem&) = delete;
		JobSystem& operator=(const JobSystem&) = delete;

		static void initialize(uint32_t threadCount = INT32_MAX);

		static void shutDown();
		static uint32_t getThreadCount();

		static void execute(Context& ctx, const std::function<void(JobArgs)>& function);
		static void execute(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& function);

	private:
		static void work(uint32_t startingQueue, Priority priority);
		friend class Context;
	};
}