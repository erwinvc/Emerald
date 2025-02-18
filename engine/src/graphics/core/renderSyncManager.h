#pragma once
#include <mutex>
#include "..\..\utils\threading\threadManager.h"
#include "..\..\utils\datastructures\buffer.h"

namespace emerald {
	using RenderCommandFn = void(*)(void*);

	// "CommandBuffer" is a fixed-size memory block for storing commands.
	class CommandBuffer {
	public:
		CommandBuffer(size_t bufferSize) : m_storage((uint32_t)bufferSize), m_capacity(bufferSize), m_writeOffset(0), m_commandCount(0) {}

		// Store RenderCommand using a little "header" (RenderCommandFn, size),
		// and then the function object data in place.
		template<typename FuncT>
		void push(const FuncT& func) {
			auto invoker = [](void* ptr) {
				using T = std::decay_t<FuncT>;
				if constexpr (std::is_trivially_destructible_v<T>) {
					// Trivial: Just call
					((T*)ptr)->operator()();
				} else {
					// Non-trivial: call and then destruct
					auto pFunc = (T*)ptr;
					(*pFunc)();
					pFunc->~T();
				}
			};

			using T = std::decay_t<FuncT>;
			constexpr size_t objectSize = sizeof(T);

			const size_t totalBytesNeeded = sizeof(RenderCommandFn) + sizeof(uint32_t) + objectSize;
			ASSERT(m_writeOffset + totalBytesNeeded <= m_capacity, "CommandBuffer out of space!");

			*(RenderCommandFn*)(m_storage.data() + m_writeOffset) = invoker;
			m_writeOffset += sizeof(RenderCommandFn);

			*(uint32_t*)(m_storage.data() + m_writeOffset) = (uint32_t)objectSize;
			m_writeOffset += sizeof(uint32_t);

			void* objectMemory = (m_storage.data() + m_writeOffset);
			new (objectMemory) T(func);
			m_writeOffset += objectSize;

			m_commandCount++;
		}

		void execute() {
			size_t readOffset = 0;
			byte* base = m_storage.data();

			for (size_t i = 0; i < m_commandCount; i++) {
				RenderCommandFn invoker = *(RenderCommandFn*)(base + readOffset);
				readOffset += sizeof(RenderCommandFn);

				uint32_t size = *(uint32_t*)(base + readOffset);
				readOffset += sizeof(uint32_t);

				void* commandObj = (base + readOffset);

				invoker(commandObj);

				readOffset += size;
			}

			clear();
		}

		void clear() {
			m_writeOffset = 0;
			m_commandCount = 0;
		}

	private:
		Buffer<byte> m_storage;
		size_t m_capacity;
		size_t m_writeOffset;
		size_t m_commandCount;
	};

	class RenderSyncManager {
	public:
		~RenderSyncManager();

		template<typename FuncT>
		void submit(FuncT&& func) {
			static_assert(std::is_invocable_v<FuncT>, "FuncT must be callable!");

			m_backBuffer->push(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		void submitFromAnyThread(FuncT&& func) {
			static_assert(std::is_invocable_v<FuncT>, "FuncT must be callable!");

			if (m_shutdown) return;

			std::lock_guard<std::mutex> lock(m_quickMutex);
			m_quickBuffer.push(std::forward<FuncT>(func));
		}

		void acquireRenderBuffer();
		void executeRenderBuffer();
		void waitForBufferAvailability();
		void submitBufferForRendering();

	private:
		CommandBuffer m_bufferA{ 1 * 1024 * 1024 }; // 1MB
		CommandBuffer m_bufferB{ 1 * 1024 * 1024 };
		CommandBuffer m_bufferC{ 1 * 1024 * 1024 };
		CommandBuffer m_quickBuffer{ 512 * 1024 }; // 512KB

		CommandBuffer* m_frontBuffer = &m_bufferA;
		CommandBuffer* m_backBuffer = &m_bufferB;
		CommandBuffer* m_pendingBuffer = nullptr;

		// For concurrency
		std::mutex m_quickMutex;
		std::mutex m_bufferMutex;
		std::condition_variable m_renderCv;
		std::condition_variable m_logicCv;
		std::unique_lock<std::mutex> m_lockGuard{ m_bufferMutex, std::defer_lock };

		bool m_bufferReady = false;
		bool m_bufferAvailable = true;
		bool m_shutdown = false;
	};
}