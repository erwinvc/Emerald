#include "eepch.h"
#include "jobSystem.h"
#include "../datastructures/vector.h"

namespace emerald {
	class JobQueue {
	public:
		void pushBack(const Job& item) {
			std::scoped_lock lock(m_mutex);
			m_queue.push_back(item);
		}

		bool popFront(Job& item) {
			std::scoped_lock lock(m_mutex);
			if (m_queue.empty())
			{
				return false;
			}
			item = std::move(m_queue.front());
			m_queue.pop_front();
			return true;
		}

		void clear() {
			std::scoped_lock lock(m_mutex);
			m_queue.clear();
		}

	private:
		std::deque<Job> m_queue;
		std::mutex m_mutex;
	};


	struct InternalState
	{
		uint32_t m_coreCount = 0;
		uint32_t m_threadCount = 0;
		UniqueRef<JobQueue[]> m_jobQueuePerThread[int(Priority::_COUNT)];
		std::atomic_bool m_shutdown = false;
		std::condition_variable m_wakeConditions[int(Priority::_COUNT)];
		std::mutex m_wakeMutexes[int(Priority::_COUNT)];
		std::atomic<uint32_t> m_nextQueue{ 0 };
		Vector<Thread*> m_threads[int(Priority::_COUNT)];

		void shutDown()
		{
			m_shutdown.store(true);
			bool running = true;
			std::thread waker([&] {
				while (running)
				{
					for (auto& cVar : m_wakeConditions)
					{
						cVar.notify_all();
					}
				}
				});

			for (int priorityIndex = 0; priorityIndex < (uint32_t)Priority::_COUNT; priorityIndex++) {
				for (auto& thread : m_threads[priorityIndex]) {
					//thread.join();
					thread->shutdown();
				}
			}
			running = false;
			waker.join();

			for (int priorityIndex = 0; priorityIndex < (uint32_t)Priority::_COUNT; priorityIndex++) {
				m_jobQueuePerThread[priorityIndex].reset();
				m_threads[priorityIndex].clear();
			}
			m_coreCount = 0;
			m_threadCount = 0;
		}
		~InternalState()
		{
			shutDown();
		}
	} static jobSystemState;

	void JobSystem::work(uint32_t startingQueue, Priority priority)
	{
		Job job;
		for (uint32_t i = 0; i < jobSystemState.m_threadCount; ++i)
		{
			JobQueue& job_queue = jobSystemState.m_jobQueuePerThread[int(priority)][startingQueue % jobSystemState.m_threadCount];
			while (job_queue.popFront(job))
			{
				JobArgs args;
				args.m_groupID = job.m_groupID;

				for (uint32_t j = job.m_groupJobOffset; j < job.m_groupJobEnd; ++j)
				{
					args.m_jobIndex = j;
					args.m_groupIndex = j - job.m_groupJobOffset;
					job.m_function(args);
				}

				job.m_ctx->counter.fetch_sub(1);
			}
			startingQueue++;
		}
	}

	void JobSystem::initialize(uint32_t threadCount) {
		if (jobSystemState.m_threadCount > 0) {
			throw std::runtime_error("Job system already initialized");
			return;
		}

		uint32_t maxThreadCount = std::max(1u, threadCount);

		jobSystemState.m_coreCount = std::thread::hardware_concurrency();

		jobSystemState.m_threadCount = std::min(maxThreadCount, std::max(1u, jobSystemState.m_coreCount - (uint32_t)ThreadType::_COUNT)); // Leave x cores for the main threads
		for (int priorityIndex = 0; priorityIndex < (uint32_t)Priority::_COUNT; priorityIndex++) {
			jobSystemState.m_jobQueuePerThread[priorityIndex].reset(new JobQueue[jobSystemState.m_threadCount]);
			jobSystemState.m_threads[priorityIndex].reserve(jobSystemState.m_threadCount);

			const Priority priority = (Priority)priorityIndex;
			for (uint32_t threadID = 0; threadID < jobSystemState.m_threadCount; ++threadID) {
				ThreadPriority threadPriority = priority == Priority::HIGH ? ThreadPriority::NORMAL : ThreadPriority::LOWEST;

				std::string name = std::format("JobSystem_{}_{}", threadPriorityToString(threadPriority, false), threadID);
				Thread* thread = ThreadManager::createAndRegisterThread(ThreadType::LOGIC, threadPriority, name, [threadID, priority]() {
					while (!jobSystemState.m_shutdown.load())
					{
						work(threadID, priority);

						std::unique_lock<std::mutex> lock(jobSystemState.m_wakeMutexes[int(priority)]);
						jobSystemState.m_wakeConditions[int(priority)].wait(lock);
					}
					});
				jobSystemState.m_threads[priorityIndex].pushBack(thread);
			}
		}
	}

	void JobSystem::shutDown()
	{
		jobSystemState.shutDown();
	}

	uint32_t JobSystem::getThreadCount()
	{
		return jobSystemState.m_threadCount;
	}

	void JobSystem::execute(Context& ctx, const std::function<void(JobArgs)>& function) {
		ctx.counter.fetch_add(1);

		Job job;
		job.m_ctx = &ctx;
		job.m_function = function;
		job.m_groupID = 0;
		job.m_groupJobOffset = 0;
		job.m_groupJobEnd = 1;

		jobSystemState.m_jobQueuePerThread[int(ctx.priority)][jobSystemState.m_nextQueue.fetch_add(1) % jobSystemState.m_threadCount].pushBack(job);
		jobSystemState.m_wakeConditions[int(ctx.priority)].notify_one();
	}

	void JobSystem::execute(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& function) {
		if (jobCount == 0 || groupSize == 0) {
			return;
		}

		const uint32_t groupCount = (jobCount + groupSize - 1) / groupSize;

		ctx.counter.fetch_add(groupCount);

		Job job;
		job.m_ctx = &ctx;
		job.m_function = function;

		for (uint32_t groupID = 0; groupID < groupCount; ++groupID)
		{
			// For each group, generate one real job:
			job.m_groupID = groupID;
			job.m_groupJobOffset = groupID * groupSize;
			job.m_groupJobEnd = std::min(job.m_groupJobOffset + groupSize, jobCount);

			jobSystemState.m_jobQueuePerThread[int(ctx.priority)][jobSystemState.m_nextQueue.fetch_add(1) % jobSystemState.m_threadCount].pushBack(job);
		}

		jobSystemState.m_wakeConditions[int(ctx.priority)].notify_all();
	}

	bool Context::isBusy() const
	{
		return counter.load() > 0;
	}

	void Context::wait()
	{
		if (isBusy())
		{
			jobSystemState.m_wakeConditions[int(priority)].notify_all();

			JobSystem::work(jobSystemState.m_nextQueue.fetch_add(1) % jobSystemState.m_threadCount, priority);

			while (isBusy())
			{
				std::this_thread::yield();
			}
		}
	}
}