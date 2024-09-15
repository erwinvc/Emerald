#pragma once
#include <mutex>
#include <functional>

namespace emerald {
	using RenderCommand = std::function<void()>;

	class RenderSyncManager {
	public:
		~RenderSyncManager();

		void acquireRenderBuffer();
		void submit(RenderCommand command);
		void submitFromAnyThread(RenderCommand command);
		void executeRenderBuffer();
		void waitForBufferAvailability();
		void submitBufferForRendering();
		void SetTempBuffer();
		bool tempBuffer = false;
	private:
		//TODO: improve buffer storage, std::vector isnt the most efficient
		std::vector<RenderCommand> bufferA, bufferB, bufferC;
		std::vector<RenderCommand>* frontBuffer = &bufferA;    // The buffer being rendered
		std::vector<RenderCommand>* backBuffer = &bufferB;     // The buffer being filled by the logic thread
		std::vector<RenderCommand>* pendingBuffer = nullptr;   // The buffer ready for rendering
		std::unique_lock<std::mutex> lockGuard{ bufferMutex, std::defer_lock }; // Guard for deferred locking
		std::mutex bufferMutex;
		std::condition_variable renderCv;
		std::condition_variable logicCv;
		bool bufferReady = false;
		bool bufferAvailable = true;
	};
}