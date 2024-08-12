#pragma once

namespace emerald {
	class Window;
	class RenderPass;
	class VertexBuffer;
	class VertexArray;
	class IndexBuffer;

	class RenderPipeline {
	private:
		std::vector<RenderPass> m_renderpasses;
		std::shared_ptr<RenderPass> m_mainPass;
		std::shared_ptr<VertexBuffer> m_vbo;
		std::shared_ptr<VertexArray> m_vao;
		std::shared_ptr<IndexBuffer> m_ibo;

		void createRenderQueue();
		void processRenderQueue();

	public:
		RenderPipeline();
		~RenderPipeline();

		void render();
		std::shared_ptr<RenderPass> mainPass() { return m_mainPass; }
	};
}