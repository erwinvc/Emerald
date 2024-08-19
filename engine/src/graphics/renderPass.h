#pragma once
#include <memory>
#include "graphics/buffers/vertexBufferLayout.h"
#include "graphics/shaders/shader.h"

namespace emerald {
	class FrameBuffer;
	class Shader;

	struct RenderPassDesc {
		std::string name;
		Ref<FrameBuffer> frameBuffer;
		Ref<Shader> shader;
	};

	class RenderPass : public RefCounted {
	private:
		RenderPassDesc m_desc;

	public:
		RenderPass(RenderPassDesc desc);
		~RenderPass();
		void clear();
		void bind();
		RenderPassDesc& descriptor() { return m_desc; }
	};
}