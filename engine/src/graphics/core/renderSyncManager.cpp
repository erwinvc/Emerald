#include "eepch.h"
#include "renderSyncManager.h"
#include "imguiProfiler/Profiler.h"
#include "utils/threading/threadManager.h"

namespace emerald {
	static bool s_shutdown = false;
	static std::vector<RenderCommand> quickBuffer;
	static std::mutex quickMutex;
	RenderSyncManager::~RenderSyncManager() {
		s_shutdown = true;
	}

	void RenderSyncManager::acquireRenderBuffer() {
		std::unique_lock<std::mutex> lock(bufferMutex);

		renderCv.wait(lock, [this] { return bufferReady; }); // Wait until there is a buffer ready for rendering

		std::swap(frontBuffer, pendingBuffer); // Swap the front buffer with the pending buffer
		bufferReady = false;
		bufferAvailable = true;

		lock.unlock();
		logicCv.notify_one();
	}

	void RenderSyncManager::submit(RenderCommand command) {
		if (s_shutdown) return;
		ASSERT(!ThreadManager::isThread(ThreadType::RENDER), "The render thread is not supposed to queue render commands");
		backBuffer->emplace_back(command);
	}

	void RenderSyncManager::submitFromAnyThread(RenderCommand command) {
		if (s_shutdown) return;

		std::lock_guard<std::mutex> lock(quickMutex);
		quickBuffer.emplace_back(command);
	}

	void RenderSyncManager::executeRenderBuffer() {
		for (const auto& cmd : *frontBuffer) {
			if (cmd) cmd();
		}

		frontBuffer->clear();

		std::lock_guard<std::mutex> lock(quickMutex);
		for (const auto& cmd : quickBuffer) {
			if (cmd) cmd();
		}
		quickBuffer.clear();
	}

	void RenderSyncManager::waitForBufferAvailability() {
		bufferMutex.lock(); // Lock mutex to safely access shared buffers
		logicCv.wait(lockGuard, [this] { return bufferAvailable; }); // Wait until a back buffer is available
	}

	void RenderSyncManager::submitBufferForRendering() {
		pendingBuffer = backBuffer; // Swap the back buffer with the pending buffer
		backBuffer = (backBuffer == &bufferA) ? ((frontBuffer == &bufferB) ? &bufferC : &bufferB) : &bufferA;

		bufferReady = true; // Mark buffer as ready for rendering and notify the render thread
		bufferAvailable = false;
		bufferMutex.unlock();
		renderCv.notify_one();
	}
}