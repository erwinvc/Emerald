#include "eepch.h"
#include "renderSyncManager.h"
#include "imguiProfiler/Profiler.h"
#include "utils/threading/threadManager.h"

namespace emerald {
	RenderSyncManager::~RenderSyncManager() {
		m_shutdown = true;
	}

	void RenderSyncManager::acquireRenderBuffer() {
		std::unique_lock<std::mutex> lock(m_bufferMutex);

		m_renderCv.wait(lock, [this] { return m_bufferReady; }); // Wait until there is a buffer ready for rendering

		std::swap(m_frontBuffer, m_pendingBuffer); // Swap the front buffer with the pending buffer
		m_bufferReady = false;
		m_bufferAvailable = true;

		lock.unlock();
		m_logicCv.notify_one();
	}

	void RenderSyncManager::executeRenderBuffer() {
		m_frontBuffer->execute();

		std::lock_guard<std::mutex> lock(m_quickMutex);
		m_quickBuffer.execute();
	}

	void RenderSyncManager::waitForBufferAvailability() {
		m_bufferMutex.lock();
		m_logicCv.wait(m_lockGuard, [this] { return m_bufferAvailable; });
	}

	void RenderSyncManager::submitBufferForRendering() {
		m_pendingBuffer = m_backBuffer; // Swap the back buffer with the pending buffer
		m_backBuffer = (m_backBuffer == &m_bufferA) ? ((m_frontBuffer == &m_bufferB) ? &m_bufferC : &m_bufferB) : &m_bufferA;

		m_bufferReady = true; // Mark buffer as ready for rendering and notify the render thread
		m_bufferAvailable = false;
		m_bufferMutex.unlock();
		m_renderCv.notify_one();
	}
}