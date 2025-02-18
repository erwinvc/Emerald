#include "OpenGLProfiler.h"
#include "CPUProfiler.h"
#include "glad/glad.h" // or your GL loader
#include <algorithm> // std::sort
#include <stdexcept> // if we want to throw
#include <cstring>
#include "utils/system/time.h"
#include "windows.h"
#include <winnt.h>

namespace ImGuiProfiler {
	void GPUProfiler::Initialize(const std::string& name, uint32_t historySize, uint32_t maxEvents) {
		m_Name = name;
		m_HistorySize = historySize;
		m_FrameIndex = 0;
		m_Paused = false;
		m_NextEventIndex.store(0);

		// Clean up if already allocated
		Shutdown();

		// Allocate ring buffer for frames
		m_pFrameData = new FrameData[historySize];
		for (uint32_t i = 0; i < historySize; i++) {
			m_pFrameData[i].Events.resize(maxEvents);
			m_pFrameData[i].NumEvents.store(0);
		}
	}

	void GPUProfiler::Shutdown() {
		if (m_pFrameData) {
			delete[] m_pFrameData;
			m_pFrameData = nullptr;
		}
		m_EventStack.clear();
	}

	void GPUProfiler::BeginEvent(const char* pName, const char* pFilePath, uint32_t lineNumber) {
		if (m_Paused || !m_pFrameData)
			return;

		// Access current frame
		FrameData& frameData = GetFrameData(m_FrameIndex);

		// Reserve a slot for this event:
		uint32_t eventIndex = frameData.NumEvents.fetch_add(1);
		if (eventIndex >= frameData.Events.size()) {
			// You might log a warning: "Too many GPU events this frame!"
			return;
		}

		GPUEvent& newEvent = frameData.Events[eventIndex];
		newEvent.pName = pName;
		newEvent.pFilePath = pFilePath;
		newEvent.LineNumber = lineNumber;
		newEvent.QueryID_Begin = 0;
		newEvent.QueryID_End = 0;
		newEvent.TicksBegin = 0;
		newEvent.TicksEnd = 0;

		newEvent.Depth = static_cast<uint32_t>(m_EventStack.size());

		// Push it onto the stack so EndEvent() can find it
		m_EventStack.push_back(eventIndex);

		// Create a new query for the GPU "begin" timestamp
		GLuint queryBegin = 0;
		glGenQueries(1, &queryBegin);
		glQueryCounter(queryBegin, GL_TIMESTAMP);

		newEvent.QueryID_Begin = queryBegin;
	}

	void GPUProfiler::EndEvent() {
		if (m_Paused || !m_pFrameData)
			return;

		// Access current frame
		FrameData& frameData = GetFrameData(m_FrameIndex);

		if (m_EventStack.empty()) {
			// Mismatched Begin/End calls, no event to end
			return;
		}

		// Get the top event index from the stack
		uint32_t topIndex = m_EventStack.back();
		m_EventStack.pop_back();

		GPUEvent& evt = frameData.Events[topIndex];
		if (evt.QueryID_Begin == 0) {
			// Means we never began this event properly
			return;
		}

		// Create a new query for the GPU "end" timestamp
		GLuint queryEnd = 0;
		glGenQueries(1, &queryEnd);
		glQueryCounter(queryEnd, GL_TIMESTAMP);

		evt.QueryID_End = queryEnd;
	}

	void GPUProfiler::Tick() {
		if (!m_pFrameData || m_Paused)
			return;

		if (m_FrameIndex)
			EndEvent();

		synchronizeGPUAndCPUTimestamps();

		// We finalize the frame that just ended: "m_FrameIndex"
		// But actually that "current" frame is about to become the "old" frame
		// So let's retrieve it first
		if (!m_Paused) {
			// Finalize the events from this frame: read query results
			FrameData& frame = GetFrameData(m_FrameIndex);

			// For each event in this frame, call glGetQueryObjectui64v to retrieve the actual times
			uint32_t numEvents = frame.NumEvents.load();
			for (uint32_t i = 0; i < numEvents; i++) {
				GPUEvent& evt = frame.Events[i];
				if (evt.QueryID_Begin == 0 || evt.QueryID_End == 0) {
					if (evt.QueryID_Begin != 0)
						glDeleteQueries(1, &evt.QueryID_Begin);
					if (evt.QueryID_End != 0)
						glDeleteQueries(1, &evt.QueryID_End);
					evt.QueryID_Begin = 0;
					evt.QueryID_End = 0;
					continue;
				}

				GLuint64 timeBegin = 0, timeEnd = 0;
				glGetQueryObjectui64v(evt.QueryID_Begin, GL_QUERY_RESULT, &timeBegin);
				glGetQueryObjectui64v(evt.QueryID_End, GL_QUERY_RESULT, &timeEnd);

				// Convert GPU nanoseconds to CPU ticks
				const double nanosToTicks = emerald::Time::getNanosecondsToTicks();
				evt.TicksBegin = uint64_t(timeBegin * nanosToTicks) + m_GPUToCPUOffset;
				evt.TicksEnd = uint64_t(timeEnd * nanosToTicks) + m_GPUToCPUOffset;

				// free the queries
				glDeleteQueries(1, &evt.QueryID_Begin);
				glDeleteQueries(1, &evt.QueryID_End);

				evt.QueryID_Begin = 0;
				evt.QueryID_End = 0;
			}

			// Optionally sort them or do something else...
			// E.g. Sort by TicksBegin if you like:
			// std::sort(frame.Events.begin(), frame.Events.begin() + numEvents, 
			//     [](auto& a, auto& b){ return a.TicksBegin < b.TicksBegin; }
			// );

			// Done finalizing. We'll move on to the next frame
		}

		// Increment frame index: the next events go in the next slot
		m_FrameIndex++;

		// Get the new "current" frame data and reset it for usage
		FrameData& newFrame = GetFrameData(m_FrameIndex);
		newFrame.NumEvents.store(0);
		// The actual Event structs remain allocated, but we'll overwrite them as we go.

		BeginEvent(m_Name.c_str());
	}

	void GPUProfiler::synchronizeGPUAndCPUTimestamps() {
		// Create timestamp queries
		GLuint queries[2];
		glGenQueries(2, queries);

		// Get CPU timestamp before
		uint64_t cpuTimeBefore;
		QueryPerformanceCounter((LARGE_INTEGER*)&cpuTimeBefore);

		// Insert GPU timestamp
		glQueryCounter(queries[0], GL_TIMESTAMP);

		// Force GPU to CPU synchronization
		GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000000);  // 1 second timeout
		glDeleteSync(fence);

		// Get CPU timestamp after
		uint64_t cpuTimeAfter;
		QueryPerformanceCounter((LARGE_INTEGER*)&cpuTimeAfter);

		// Get GPU timestamp
		GLuint64 gpuTicks;
		glGetQueryObjectui64v(queries[0], GL_QUERY_RESULT, &gpuTicks);

		// Convert GPU nanoseconds to CPU ticks
		const double nanosToTicks = emerald::Time::getNanosecondsToTicks();
		uint64_t gpuTicksConverted = uint64_t(gpuTicks * nanosToTicks);

		// CPU timestamp at GPU timestamp insertion is approximately the midpoint
		uint64_t cpuMidpoint = (cpuTimeBefore + cpuTimeAfter) / 2;

		// Calculate offset between GPU and CPU timeline
		m_GPUToCPUOffset = int64_t(cpuMidpoint) - int64_t(gpuTicksConverted);

		// Cleanup
		glDeleteQueries(2, queries);
	}
}