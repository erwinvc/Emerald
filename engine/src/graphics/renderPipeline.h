#pragma once
#include "graphics/renderPass.h"

namespace emerald {
	class Window;
	class VertexBuffer;
	class VertexArray;
	class IndexBuffer;

	class RenderPipeline {
	private:
		std::vector<RenderPass> m_renderpasses;
		Ref<RenderPass> m_mainPass;
		Ref<VertexBuffer> m_vbo;
		Ref<VertexArray> m_vao;
		Ref<IndexBuffer> m_ibo;

		void createRenderQueue();
		void processRenderQueue();

	public:
		RenderPipeline();
		~RenderPipeline();

		void render();
		Ref<RenderPass> mainPass() { return m_mainPass; }
	};
}