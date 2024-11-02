#pragma once
#include "windows.h"
#include "stdincl.h"
#include <functional>
#include <stdexcept>

namespace emerald {
	enum FiberLifetime {
		ONCE,
		LOOPING
	};

	//Fiber aka coroutine class which provides a lightweight cooperatively scheduled thread.
	class Fiber {
	public:
		using FiberFunc = std::function<void()>;

	private:
		static Fiber* s_currentFiber; //Tracks the currently executing fiber, enabling yield and sleep functionality.

		std::string m_name;
		FiberFunc m_function;

		FiberLifetime m_lifeTime;
		LPVOID m_handle = nullptr;
		bool m_isSleeping = false;
		float m_wakeTime = 0;
		bool m_completed = false;

	public:
		Fiber(const std::string& name, FiberLifetime lifeTime, FiberFunc func)
			: m_name(name), m_lifeTime(lifeTime), m_function(func) {
			m_handle = CreateFiber(0, Fiber::fiberFunctionWrapper, this);
		}

		~Fiber() { ::DeleteFiber(m_handle); }

		bool isSleeping() const { return m_isSleeping; }
		bool isCompleted() const { return m_completed; }
		const std::string& getName() const { return m_name; }
		LPVOID handle() const { return m_handle; }

		void tryWake();

		void run() {
			s_currentFiber = this;
			SwitchToFiber(m_handle);
		}

		void kill() {
			m_completed = true;
		}

		static void __stdcall fiberFunctionWrapper(LPVOID lpFiberParameter);
		static void sleep(float waitTime); // Sleeps the current fiber for a specified duration, then yields control.
		static void yieldFiber(); // Yields control from the current fiber back to the fiber manager for scheduling.
	};

	// Manages the execution, suspension, and resumption of fibers, including cleanup of completed fibers.
	class FiberManager {
	private:
		static std::vector<std::unique_ptr<Fiber>> s_fibers;
		static LPVOID s_mainFiber;

		friend class Fiber;

	public:
		static LPVOID getMainFiber() {
			return s_mainFiber;
		}

		static void initializeMainFiber() {
			if (s_mainFiber) throw std::runtime_error("Main fiber has already been initialized");
			s_mainFiber = ConvertThreadToFiber(nullptr);
		}

		static void registerFiber(const std::string& name, FiberLifetime lifeTime, Fiber::FiberFunc func) {
			s_fibers.emplace_back(std::make_unique<Fiber>(name, lifeTime, func));
		}

		static void run() {
			for (auto& fiber : s_fibers) {
				if (!fiber->isCompleted()) {
					if (!fiber->isSleeping()) {
						fiber->run();
					} else {
						fiber->tryWake();
					}
				}

			}

			//Delete completed fibers. This needs to be done on the main fiber.
			auto newEnd = std::remove_if(s_fibers.begin(), s_fibers.end(),
				[](const std::unique_ptr<Fiber>& fiber) {
					return fiber->isCompleted();
				});
			s_fibers.erase(newEnd, s_fibers.end());
		}
	};
}