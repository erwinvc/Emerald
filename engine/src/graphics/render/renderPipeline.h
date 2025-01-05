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

		Ref<Shader> m_shadowShader;
		Ref<Material> m_shadowMaterial;
		Ref<RenderPass> m_shadowPass;
		Ref<FrameBuffer> m_shadowFramebuffer;
		Ref<FrameBuffer> m_blurFBOHorizontal;
		Ref<FrameBuffer> m_blurFBOVertical;
		
		glm::mat4 m_lightView;
		glm::mat4 m_lightProjection;

		glm::vec3 m_lightPosition = glm::vec3(1.0f, 19.0f, 4.0f);
		glm::vec3 m_lightTarget = glm::vec3(0.0f, 0.0f, 0.0f);
		float m_orthoSize = 210.0f;
		float m_zNear = 1.0f;
		float m_zFar = 145.0f;

	public:
		RenderPipeline();
		~RenderPipeline();

		void updateLightMatrices();
		void onImGuiRender();
		void render();
		const Ref<Texture>& getFinalTexture();
		static inline Ref<Shader> shader;
	};
}