#pragma once
#include <memory>
#include "graphics/buffers/vertexBufferLayout.h"

namespace emerald {
	class FrameBuffer;
	class Shader;

	struct RenderPassDesc {
		std::string name;
		std::shared_ptr<FrameBuffer> frameBuffer;
		std::shared_ptr<Shader> shader;
	};

	class RenderPass {
	private:
		RenderPassDesc m_desc;

	public:
		RenderPass(RenderPassDesc desc);
		~RenderPass();
		void clear();
		void bind();
		const RenderPassDesc& descriptor() { return m_desc; }
	};
}