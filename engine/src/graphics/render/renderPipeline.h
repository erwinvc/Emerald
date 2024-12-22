#pragma once
#include "graphics/render/renderPass.h"
#include "graphics/render/material.h"
#include "graphics/textures/texture.h"

namespace emerald {
	class Window;
	class VertexBuffer;
	class VertexArray;
	class IndexBuffer;

	class RenderPipeline {
	private:
		std::vector<RenderPass> m_renderpasses;
		Ref<FrameBuffer> m_resolveFramebuffer;
		Ref<RenderPass> m_mainPass;
		Ref<VertexBuffer> m_vbo;
		Ref<VertexArray> m_vao;
		Ref<IndexBuffer> m_ibo;
		Ref<Material> m_material;

		void createRenderQueue();
		void processRenderQueue();

	public:
		RenderPipeline();
		~RenderPipeline();

		void render();
		const Ref<Texture>& getFinalTexture();
		static inline Ref<Shader> shader;
	};
}