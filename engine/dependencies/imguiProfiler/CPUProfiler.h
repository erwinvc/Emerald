#pragma once
#include <algorithm>
#include <atomic>
#include <vector>
#include <mutex>
#include <span>
#include <assert.h>


namespace ImGuiProfiler {
#define check(op, ...) assert(op)
#define checkf(op, ...) assert(op)
#define VERIFY_HR(op) assert(SUCCEEDED(op))

#define _STRINGIFY(a) #a
#define STRINGIFY(a) _STRINGIFY(a)
#define CONCAT_IMPL( x, y ) x##y
#define MACRO_CONCAT( x, y ) CONCAT_IMPL( x, y )
#define _NUMOF(x) (uint32_t)(((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x]))))))
#define min(a,b) (((a) < (b)) ? (a) : (b))

	using uint64 = uint64_t;
	using uint32 = uint32_t;
	using uint16 = uint16_t;
	template<typename T>
	using Span = std::span<T>;

	struct URange {
		uint32 Begin;
		uint32 End;
	};

	// Simple Linear Allocator
	class LinearAllocator {
	public:
		explicit LinearAllocator(uint32 size)
			: m_pData(new char[size]), m_Size(size), m_Offset(0) {
		}

		~LinearAllocator() {
			delete[] m_pData;
		}

		LinearAllocator(LinearAllocator&) = delete;
		LinearAllocator& operator=(LinearAllocator&) = delete;

		void Reset() {
			m_Offset = 0;
		}

		template<typename T, typename... Args>
		T* Allocate(Args... args) {
			void* pData = Allocate(sizeof(T));
			T* pValue = new (pData) T(std::forward<Args>(args)...);
			return pValue;
		}

		void* Allocate(uint32 size) {
			uint32 offset = m_Offset.fetch_add(size);
			check(offset + size <= m_Size);
			return m_pData + offset;
		}

		const char* String(const char* pStr) {
			uint32 len = (uint32)strlen(pStr) + 1;
			char* pData = (char*)Allocate(len);
			strcpy_s(pData, len, pStr);
			return pData;
		}

	private:
		char* m_pData;
		uint32 m_Size;
		std::atomic<uint32> m_Offset;
	};

	// CPU Profiler
	// Also responsible for updating GPU profiler
	// Also responsible for drawing HUD
	class CPUProfiler {
	public:
		void Initialize(std::string name, uint32 historySize, uint32 maxEvents);
		void Shutdown();

		// Start and push an event on the current thread
		void BeginEvent(const char* pName, const char* pFilePath = nullptr, uint32 lineNumber = 0);

		// End and pop the last pushed event on the current thread
		void EndEvent();

		// Resolve the last frame and advance to the next frame.
		// Call at the START of the frame.
		void Tick();

		// Initialize a thread with an optional name
		void RegisterThread(const char* pName = nullptr);

		// Struct containing all sampling data of a single frame
		struct EventData {
			static constexpr uint32 ALLOCATOR_SIZE = 1 << 14;

			EventData()
				: Allocator(ALLOCATOR_SIZE) {
			}

			// Structure representating a single event
			struct Event {
				const char* pName = "";		// Name of the event
				const char* pFilePath = nullptr;	// File path of file in which this event is recorded
				uint64		TicksBegin = 0;		// The ticks at the start of this event
				uint64		TicksEnd = 0;		// The ticks at the end of this event
				uint32		LineNumber : 16;		// Line number of file in which this event is recorded
				uint32		ThreadIndex : 11;		// Thread Index of the thread that recorderd this event
				uint32		Depth : 5;		// Depth of the event
			};

			std::vector<Span<const Event>>	EventsPerThread;	// Events per thread of the frame
			std::vector<Event>				Events;				// All events of the frame
			LinearAllocator					Allocator;			// Scratch allocator storing all dynamic allocations of the frame
			std::atomic<uint32>				NumEvents = 0;		// The number of events
		};


		// Thread-local storage to keep track of current depth and event stack
		struct TLS {
			static constexpr int MAX_STACK_DEPTH = 32;

			template<typename T, uint32 N>
			struct FixedStack {
			public:
				T& Pop() {
					check(Depth > 0);
					--Depth;
					return StackData[Depth];
				}

				T& Push() {
					Depth++;
					check(Depth < _NUMOF(StackData));
					return StackData[Depth - 1];
				}

				T& Top() {
					check(Depth > 0);
					return StackData[Depth - 1];
				}

				uint32 GetSize() const { return Depth; }

			private:
				uint32 Depth = 0;
				T StackData[N]{};
			};


			FixedStack<uint32, MAX_STACK_DEPTH> EventStack;
			uint32								ThreadIndex = 0;
			bool								IsInitialized = false;
		};

		// Structure describing a registered thread
		struct ThreadData {
			char		Name[128]{};
			uint32		ThreadID = 0;
			uint32		Index = 0;
			const TLS* pTLS = nullptr;
		};

		URange GetFrameRange() const {
			uint32 begin = m_FrameIndex - min(m_FrameIndex, m_HistorySize) + 1;
			uint32 end = m_FrameIndex;
			return URange(begin, end);
		}

		Span<const EventData::Event> GetEventsForThread(const ThreadData& thread, uint32 frame) const {
			//check(frame >= GetFrameRange().Begin && frame < GetFrameRange().End);
			const EventData& data = m_pEventData[frame % m_HistorySize];
			if (thread.Index < data.EventsPerThread.size())
				return data.EventsPerThread[thread.Index];
			return {};
		}

		// Get the ticks range of the history
		void GetHistoryRange(uint64& ticksMin, uint64& ticksMax) const {
			URange range = GetFrameRange();
			ticksMin = GetData(range.Begin).Events[0].TicksBegin;
			ticksMax = GetData(range.End).Events[0].TicksEnd;
		}

		Span<const ThreadData> GetThreads() const { return m_ThreadData; }

		void SetPaused(bool paused) { m_QueuedPaused = paused; }
		bool IsPaused() const { return m_Paused; }

	private:
		// Retrieve thread-local storage without initialization
		static TLS& GetTLSUnsafe() {
			static thread_local TLS tls;
			return tls;
		}

		// Retrieve the thread-local storage
		TLS& GetTLS() {
			TLS& tls = GetTLSUnsafe();
			if (!tls.IsInitialized)
				RegisterThread();
			return tls;
		}

		// Return the sample data of the current frame
		EventData& GetData() { return GetData(m_FrameIndex); }
		EventData& GetData(uint32 frameIndex) { return m_pEventData[frameIndex % m_HistorySize]; }
		const EventData& GetData(uint32 frameIndex)	const { return m_pEventData[frameIndex % m_HistorySize]; }

		std::mutex				m_ThreadDataLock;				// Mutex for accesing thread data
		std::vector<ThreadData> m_ThreadData;					// Data describing each registered thread

		EventData* m_pEventData = nullptr;	// Per-frame data
		uint32					m_HistorySize = 0;		// History size
		uint32					m_FrameIndex = 0;		// The current frame index
		bool					m_Paused = false;	// The current pause state
		bool					m_QueuedPaused = false;	// The queued pause state
		std::string				m_Name = "";
	};
}