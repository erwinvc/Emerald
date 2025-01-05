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

	struct Context
	{
		std::atomic<uint32_t> counter{ 0 };
		Priority priority = Priority::HIGH;
	};

	struct JobArgs
	{
		uint32_t m_jobIndex;	
		uint32_t m_groupID;	
		uint32_t m_groupIndex;
	};

	struct Job {
		std::function<void(JobArgs)> m_function;
		Context* m_ctx;
		uint32_t m_groupID;
		uint32_t m_groupJobOffset;
		uint32_t m_groupJobEnd;
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

		static void dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& function);

		static uint32_t dispatchGroupCount(uint32_t jobCount, uint32_t groupSize);
		static bool isBusy(const Context& ctx);
		static void wait(const Context& ctx);

	private:
	
	};
}