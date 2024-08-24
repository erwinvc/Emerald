#include "eepch.h"
#include "renderSyncManager.h"
#include "imguiProfiler/Profiler.h"

namespace emerald {
	std::vector<Command> tempBufferr;
	void RenderSyncManager::SetTempBuffer() {
		tempBuffer = true;
		tempBufferr.clear();
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

	void RenderSyncManager::submit(Command command) {
		ASSERT(!ThreadManager::isThread(ThreadType::RENDER), "The render thread is not supposed to queue render commands");
		if (tempBuffer)tempBufferr.emplace_back(command);
		else backBuffer->emplace_back(command);
	}

	void RenderSyncManager::submitFromAnyThread(Command command) {
		if (tempBuffer)tempBufferr.emplace_back(command);
		else backBuffer->emplace_back(command);
	}

	void RenderSyncManager::executeRenderBuffer() {
		if (tempBuffer) {
			for (const auto& cmd : tempBufferr) {
				cmd();
			}
			tempBuffer = false;
			return;
		}
		for (const auto& cmd : *frontBuffer) {
			cmd();
		}

		frontBuffer->clear();
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