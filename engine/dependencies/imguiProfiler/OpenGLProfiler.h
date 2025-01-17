#pragma once
#include "CPUProfiler.h"


namespace ImGuiProfiler {
	using GLuint = unsigned int;

	class GPUProfiler
	{
	public:
		struct GPUEvent
		{
			// Event name, file, line, etc., as needed:
			const char* pName = nullptr;
			const char* pFilePath = nullptr;
			uint32_t    LineNumber = 0;

			uint32_t    Depth = 0; // new!

			// The actual GPU timestamp queries:
			GLuint      QueryID_Begin = 0;
			GLuint      QueryID_End = 0;

			// Once retrieved, store them as 64-bit "ticks" (nanoseconds).
			uint64_t    TicksBegin = 0;
			uint64_t    TicksEnd = 0;
		};

		// Per-frame data: we keep a dynamic array of events that occurred this frame.
		struct FrameData
		{
			std::vector<GPUEvent> Events;
			std::atomic<uint32_t> NumEvents{ 0 };
		};

		GPUProfiler() = default;
		~GPUProfiler() = default;

		// Initialize with e.g. "GPU" name, number of frames in history, max events per frame
		void Initialize(const std::string& name, uint32_t historySize, uint32_t maxEvents);
		void Shutdown();

		// Start an event: record the name, create a "begin" query
		void BeginEvent(const char* pName, const char* pFilePath = nullptr, uint32_t lineNumber = 0);

		// End event: create an "end" query
		void EndEvent();

		// Called once per frame to:
		//  1) finalize the old frame’s queries
		//  2) increment frame index
		//  3) reset the buffer for the new frame
		void Tick();

		// Access the ring-buffered frames:
		const FrameData& GetFrameData(uint32_t frameIndex) const {
			return m_pFrameData[frameIndex % m_HistorySize];
		}
		FrameData& GetFrameData(uint32_t frameIndex) {
			return m_pFrameData[frameIndex % m_HistorySize];
		}

		uint32_t GetCurrentFrameIndex() const { return m_FrameIndex; }
		uint32_t GetHistorySize() const { return m_HistorySize; }

		// Example: retrieve [Begin,End) range of frames for the timeline
		// The oldest fully captured frame is (m_FrameIndex - m_HistorySize).
		// The newest is (m_FrameIndex - 1).
		struct URange { uint32_t Begin, End; };
		URange GetFrameRange() const {
			const uint32_t begin = (m_FrameIndex > m_HistorySize)
				? (m_FrameIndex - m_HistorySize)
				: 0; // if not filled
			const uint32_t end = m_FrameIndex;
			return { begin, end };
		}

		// Pause/unpause if needed
		void SetPaused(bool paused) { m_Paused = paused; }
		bool IsPaused() const { return m_Paused; }
		void synchronizeGPUAndCPUTimestamps();

	private:
		int64_t m_GPUToCPUOffset = 0;

		std::string m_Name;
		uint32_t    m_HistorySize = 0;
		uint32_t    m_FrameIndex = 0;
		bool        m_Paused = false;

		FrameData* m_pFrameData = nullptr;

		// For the current frame: index to the next event in the array
		std::atomic<uint32_t> m_NextEventIndex{ 0 };
		std::vector<uint32_t> m_EventStack;
	};
}