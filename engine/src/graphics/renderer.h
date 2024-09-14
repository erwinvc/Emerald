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
		static void setTempBuffer();
		static void acquireRenderBuffer();
		static void waitForBufferAvailability();
		static void submitBufferForRendering();
		static void flushCommandBufferOnThisThread();

		static void beginRenderPass(const Ref<RenderPass>& renderPass);
		static void endRenderPass();

		static void executeCommandBuffer();

		static void submit(RenderCommand command);
		static void submitFromAnyThread(RenderCommand command);
		static void drawIndexed(uint32_t count, PrimitiveType type);
	};
}