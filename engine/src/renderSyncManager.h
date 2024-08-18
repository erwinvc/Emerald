#pragma once

namespace emerald {
	using Command = std::function<void()>;

	class RenderSyncManager {
	public:
		void acquireRenderBuffer();
		void submit(Command command);
		void executeRenderBuffer();
		void waitForBufferAvailability();
		void submitBufferForRendering();
		void SetTempBuffer();
		bool tempBuffer = false;
	private:
		//TODO: improve buffer storage, std::vector isnt the most efficient
		std::vector<Command> bufferA, bufferB, bufferC;
		std::vector<Command>* frontBuffer = &bufferA;    // The buffer being rendered
		std::vector<Command>* backBuffer = &bufferB;     // The buffer being filled by the logic thread
		std::vector<Command>* pendingBuffer = nullptr;   // The buffer ready for rendering
		std::unique_lock<std::mutex> lockGuard{ bufferMutex, std::defer_lock }; // Guard for deferred locking
		std::mutex bufferMutex;
		std::condition_variable renderCv;
		std::condition_variable logicCv;
		bool bufferReady = false;
		bool bufferAvailable = true;
	};
}