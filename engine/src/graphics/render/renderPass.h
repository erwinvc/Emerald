#pragma once
#include <memory>
#include "graphics/buffers/vertexBufferLayout.h"
#include "graphics/shaders/shader.h"
#include "utils/misc/flags.h"
#include "../misc/types.h"

namespace emerald {
	class FrameBuffer;
	class Shader;

	struct RenderPassDesc {
		std::string name;
		Ref<FrameBuffer> frameBuffer;
		Flags<3> clearMask;
		Ref<Shader> shader;
	};

	class RenderPass : public RefCounted {
	public:
		RenderPass(RenderPassDesc desc);
		~RenderPass();
		void clear();
		void bind();
		RenderPassDesc& descriptor() { return m_desc; }

	private:
		RenderPassDesc m_desc;
	};
}