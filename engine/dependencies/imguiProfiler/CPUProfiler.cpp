#include "CPUProfiler.h"
#include "windows.h"
#include "winnt.h"

namespace ImGuiProfiler {

	void CPUProfiler::Initialize(std::string name, uint32 historySize, uint32 maxEvents) {
		m_Name = name;

		Shutdown();

		m_pEventData = new EventData[historySize];
		m_HistorySize = historySize;

		for (uint32 i = 0; i < historySize; ++i)
			m_pEventData[i].Events.resize(maxEvents);

		for (uint32 i = 0; i < m_HistorySize; ++i)
			m_pEventData[i].EventsPerThread.resize((uint8_t)emerald::ProfilerThreadType::_COUNT);
	}


	void CPUProfiler::Shutdown() {
		delete[] m_pEventData;
	}


	void CPUProfiler::BeginEvent(const char* pName, const char* pFilePath, uint32 lineNumber) {
		if (m_Paused)
			return;

		EventData& data = GetData();
		uint32 newIndex = data.NumEvents.fetch_add(1);
		check(newIndex < data.Events.size());

		TLS& tls = GetTLS();

		EventData::Event& newEvent = data.Events[newIndex];
		newEvent.Depth = tls.EventStack.GetSize();
		newEvent.Type = tls.Type;
		newEvent.pName = data.Allocator.String(pName);
		newEvent.pFilePath = pFilePath;
		newEvent.LineNumber = lineNumber;
		QueryPerformanceCounter((LARGE_INTEGER*)(&newEvent.TicksBegin));

		tls.EventStack.Push() = newIndex;
	}


	// End and pop the last pushed event on the current thread
	void CPUProfiler::EndEvent() {
		if (m_Paused)
			return;

		EventData::Event& event = GetData().Events[GetTLS().EventStack.Pop()];
		QueryPerformanceCounter((LARGE_INTEGER*)(&event.TicksEnd));
	}


	void CPUProfiler::Tick() {
		m_Paused = m_QueuedPaused;
		if (m_Paused)
			return;

		if (m_FrameIndex)
			EndEvent();

		// Check if all threads have ended all open sample events
		for (auto& threadData : m_ThreadData)
			check(threadData.pTLS->EventStack.GetSize() == 0);

		// Sort the events by thread and group by thread
		EventData& frame = GetData();
		std::vector<EventData::Event>& events = frame.Events;
		std::sort(events.begin(), events.begin() + frame.NumEvents, [](const EventData::Event& a, const EventData::Event& b) {
			return (uint16_t)a.Type < (uint16_t)b.Type;
		});

		URange eventRange(0, 0);
		for (uint16_t i = 0; i < (uint16_t)emerald::ProfilerThreadType::_COUNT; i++) {
			emerald::ProfilerThreadType type = (emerald::ProfilerThreadType)i;

			//for (uint32 threadIndex = 0; threadIndex < (uint32)m_ThreadData.size(); ++threadIndex) {
				// Reset event range for this thread
			eventRange.End = eventRange.Begin;

			// If we've processed all events or this thread has no events
			if (eventRange.Begin >= frame.NumEvents ||
				events[eventRange.Begin].Type != type) {
				// Set empty span for this thread
				frame.EventsPerThread[(uint16_t)type] = Span<const EventData::Event>();
				continue;
			}

			// Process events for this thread
			while (eventRange.End < frame.NumEvents &&
				events[eventRange.End].Type == type)
				++eventRange.End;

			frame.EventsPerThread[(uint16_t)type] = Span<const EventData::Event>(
				&events[eventRange.Begin],
				eventRange.End - eventRange.Begin
			);
			eventRange.Begin = eventRange.End;
		}

		++m_FrameIndex;

		EventData& newData = GetData();
		newData.Allocator.Reset();
		newData.NumEvents = 0;

		BeginEvent(m_Name.c_str());
	}


	void CPUProfiler::RegisterThread(const char* pName, emerald::ProfilerThreadType type) {
		if (type == emerald::ProfilerThreadType::IGNORED) return;

		TLS& tls = GetTLSUnsafe();
		check(!tls.IsInitialized);
		tls.IsInitialized = true;
		std::scoped_lock lock(m_ThreadDataLock);
		tls.ThreadIndex = (uint32)m_ThreadData.size();
		tls.Type = type;
		ThreadData& data = m_ThreadData.emplace_back();

		// If the name is not provided, retrieve it using GetThreadDescription()
		if (pName) {
			strcpy_s(data.Name, ARRAYSIZE(data.Name), pName);
		} else {
			PWSTR pDescription = nullptr;
			VERIFY_HR(::GetThreadDescription(GetCurrentThread(), &pDescription));
			size_t converted = 0;
			check(wcstombs_s(&converted, data.Name, ARRAYSIZE(data.Name), pDescription, ARRAYSIZE(data.Name)) == 0);
		}
		data.ThreadID = GetCurrentThreadId();
		data.pTLS = &tls;
		//data.Index = (uint32)m_ThreadData.size() - 1;
		data.Type = type;

		std::sort(m_ThreadData.begin(), m_ThreadData.end(), [](const ThreadData& a, const ThreadData& b) {
			return (uint16_t)a.Type < (uint16_t)b.Type;
		});
	}
}