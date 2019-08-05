#include "stdafx.h"

void GLFiberManager::Initialize() {
	if (m_initialized) return;
	ConvertThreadToFiber(nullptr);
	m_mainFiber = GetCurrentFiber();
	m_initialized = true;
	LOG("[~rThreads~x] GLFiber created");
}

void GLFiberManager::Tick() {
	if (!m_currentFiber || !m_initialized) return;
	for (Fiber& f : m_fibers) {
		if (glfwGetTime() >= f.m_wakeTime) {
			SwitchToFiber(f.m_fiber);
		}
	}
	m_currentFiber = m_currentFiber->m_nextFiber;
}

void GLFiberManager::Sleep(float waitTime) {
	auto& fiber = find_if(m_fibers.begin(), m_fibers.end(), [](const Fiber& fiber) -> bool { return fiber.m_fiber == GetCurrentFiber(); });
	if (fiber != m_fibers.end()) {
		fiber->m_wakeTime = (float)glfwGetTime() + waitTime;
		SwitchToFiber(m_mainFiber);
	}
}

void GLFiberManager::GoToMainFiber() {
	SwitchToFiber(m_mainFiber);
}

void GLFiberManager::Delete(String name) {
	for (Fiber& fiber : m_fibers) {
		if (fiber.m_name.compare(name) == 0) {
			DeleteFiber(fiber.m_fiber);
			LOG("[~rThreads~x] Deleted ~1%s~x fiber", fiber.m_name);
			break;
		}
	}
}

void GLFiberManager::Cleanup() {
	if (!m_cleaned) {
		for (Fiber& fiber : m_fibers) {
			DeleteFiber(fiber.m_fiber);
			LOG("[~rThreads~x] Deleted ~1%s~x fiber", fiber.m_name);
		}
		//LOG_DEBUG("[Threads] %s deleted", m_name);
		m_cleaned = true;
	}
}