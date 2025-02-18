#pragma once
#include "renderSyncManager.h"

namespace emerald {
	class RenderPass;

	enum class PrimitiveType {
		POINTS = 0x0000,
		LINES = 0x0001,
		LINE_LOOP = 0x0002,
		LINE_STRIP = 0x0003,
		TRIANGLES = 0x0004,
		TRIANGLE_STRIP = 0x0005,
		TRIANGLE_FAN = 0x0006,
	};

	class Renderer {
	public:
		template<typename FuncT>
		static void submit(FuncT&& func) {
			s_renderSyncManager.submit(std::forward<FuncT>(func));
		}

		template<typename FuncT>
		static void submitFromAnyThread(FuncT&& func) {
			s_renderSyncManager.submitFromAnyThread(std::forward<FuncT>(func));
		}

		static void acquireRenderBuffer();
		static void waitForBufferAvailability();
		static void submitBufferForRendering();
		static void flushRenderCommands();

		static void beginRenderPass(const Ref<RenderPass>& renderPass);
		static void endRenderPass();

		static void executeCommandBuffer();

		static void drawIndexed(uint32_t count, PrimitiveType type);

	private:
		static inline RenderSyncManager s_renderSyncManager;
	};
}