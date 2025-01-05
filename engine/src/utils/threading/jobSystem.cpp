#include "eepch.h"
#include "jobSystem.h"
#include "../datastructures/vector.h"

namespace emerald {
	class JobQueue : public RefCounted
	{
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
	} static internal_state;

	void work(uint32_t startingQueue, Priority priority)
	{
		Job job;
		for (uint32_t i = 0; i < internal_state.m_threadCount; ++i)
		{
			JobQueue& job_queue = internal_state.m_jobQueuePerThread[int(priority)][startingQueue % internal_state.m_threadCount];
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
		if (internal_state.m_threadCount > 0) {
			throw std::runtime_error("Job system already initialized");
			return;
		}

		uint32_t maxThreadCount = std::max(1u, maxThreadCount);

		internal_state.m_coreCount = std::thread::hardware_concurrency();

		internal_state.m_threadCount = std::min(maxThreadCount, std::max(1u, internal_state.m_coreCount - (uint32_t)ThreadType::_COUNT)); // Leave x cores for the main threads
		for (int priorityIndex = 0; priorityIndex < (uint32_t)Priority::_COUNT; priorityIndex++) {
			internal_state.m_jobQueuePerThread[priorityIndex].reset(new JobQueue[internal_state.m_threadCount]);
			internal_state.m_threads[priorityIndex].reserve(internal_state.m_threadCount);

			const Priority priority = (Priority)priorityIndex;
			for (uint32_t threadID = 0; threadID < internal_state.m_threadCount; ++threadID) {
				ThreadPriority threadPriority = priority == Priority::HIGH ? ThreadPriority::NORMAL : ThreadPriority::LOWEST;

				std::string name = std::format("JobSystem_{}_{}", threadPriorityToString(threadPriority, false), threadID);
				Thread* thread = ThreadManager::createAndRegisterThread(ThreadType::LOGIC, threadPriority, name, [threadID, priority]() {
					while (!internal_state.m_shutdown.load())
					{
						work(threadID, priority);

						std::unique_lock<std::mutex> lock(internal_state.m_wakeMutexes[int(priority)]);
						internal_state.m_wakeConditions[int(priority)].wait(lock);
					}
					});
				internal_state.m_threads[priorityIndex].pushBack(thread);
			}
		}
	}

	void JobSystem::shutDown()
	{
		internal_state.shutDown();
	}

	uint32_t JobSystem::getThreadCount()
	{
		return internal_state.m_threadCount;
	}

	void JobSystem::execute(Context& ctx, const std::function<void(JobArgs)>& function) {
		ctx.counter.fetch_add(1);

		Job job;
		job.m_ctx = &ctx;
		job.m_function = function;
		job.m_groupID = 0;
		job.m_groupJobOffset = 0;
		job.m_groupJobEnd = 1;

		internal_state.m_jobQueuePerThread[int(ctx.priority)][internal_state.m_nextQueue.fetch_add(1) % internal_state.m_threadCount].pushBack(job);
		internal_state.m_wakeConditions[int(ctx.priority)].notify_one();
	}

	void JobSystem::dispatch(Context& ctx, uint32_t jobCount, uint32_t groupSize, const std::function<void(JobArgs)>& function) {
		if (jobCount == 0 || groupSize == 0)
		{
			return;
		}

		const uint32_t groupCount = dispatchGroupCount(jobCount, groupSize);

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

			internal_state.m_jobQueuePerThread[int(ctx.priority)][internal_state.m_nextQueue.fetch_add(1) % internal_state.m_threadCount].pushBack(job);
		}

		internal_state.m_wakeConditions[int(ctx.priority)].notify_all();
	}

	uint32_t JobSystem::dispatchGroupCount(uint32_t jobCount, uint32_t groupSize)
	{
		// Calculate the amount of job groups to dispatch (overestimate, or "ceil"):
		return (jobCount + groupSize - 1) / groupSize;
	}

	bool JobSystem::isBusy(const Context& ctx)
	{
		return ctx.counter.load() > 0;
	}

	void JobSystem::wait(const Context& ctx)
	{
		if (isBusy(ctx))
		{
			internal_state.m_wakeConditions[int(ctx.priority)].notify_all();

			work(internal_state.m_nextQueue.fetch_add(1) % internal_state.m_threadCount, ctx.priority);

			while (isBusy(ctx))
			{
				std::this_thread::yield();
			}
		}
	}
}