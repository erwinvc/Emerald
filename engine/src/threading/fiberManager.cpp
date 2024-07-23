#include "eepch.h"
#include "fiberManager.h"
#include <vector>
#include "application.h"

namespace emerald {
	Fiber* Fiber::s_currentFiber = nullptr;
	std::vector<std::unique_ptr<Fiber>> FiberManager::s_fibers;
	LPVOID FiberManager::s_mainFiber = nullptr;

	void Fiber::tryWake() {
		if (!m_isSleeping) return;
		if (App->getTime() >= m_wakeTime) {
			m_isSleeping = false;
		}
	}

	void __stdcall Fiber::fiberFunctionWrapper(LPVOID lpFiberParameter) {
		Fiber* fiber = (Fiber*)lpFiberParameter;
		if (fiber->m_lifeTime == FiberLifetime::LOOPING) {
			while (true) {
				fiber->m_function();
				Fiber::yieldFiber();
			}
		} else {
			fiber->m_function();
		}
		fiber->m_completed = true;
		Fiber::yieldFiber();
	}

	void Fiber::sleep(float waitTime) {
		if (s_currentFiber) {
			s_currentFiber->m_isSleeping = true;
			s_currentFiber->m_wakeTime = (float)App->getTime() + (waitTime / 1000.0f);
		}
		yieldFiber();
	}

	void Fiber::yieldFiber() {
		SwitchToFiber(FiberManager::s_mainFiber);
	}
}