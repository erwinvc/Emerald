#pragma once
#include <memory>

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

	struct RendererData {
		std::shared_ptr<RenderPass> m_activeRenderPass;
	};

	class Renderer {
	public:
		static void beginRenderPass(std::shared_ptr<RenderPass> renderPass);
		static void endRenderPass();

		static void bindDefaultFBO();

		static void drawIndexed(uint32_t count, PrimitiveType type, bool depthTest);
	};
}