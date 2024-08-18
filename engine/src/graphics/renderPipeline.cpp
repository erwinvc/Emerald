#include "eepch.h"
#include "renderPipeline.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "renderer.h"
#include "graphics/shaders/shader.h"
#include "buffers/indexBuffer.h"
#include "vertexArray.h"
#include "glError.h"

namespace emerald {
	RenderPipeline::RenderPipeline() {
		FramebufferDesc mainfbDesc;
		mainfbDesc.width = App->getWidth();
		mainfbDesc.height = App->getHeight();
		mainfbDesc.attachments = { {"Main", TextureFormat::RGBA} };
		mainfbDesc.clearColor = { 0.5f, 0.7f, 1.0f, 1.0f };
		mainfbDesc.name = "MainFB";

		RenderPassDesc mainPassDesc;
		mainPassDesc.frameBuffer = FrameBuffer::create(mainfbDesc);
		mainPassDesc.shader = Ref<Shader>::create("Geometry", "res/shaders/geometry");
		m_mainPass = std::make_shared<RenderPass>(mainPassDesc);

		struct Vertex {
			glm::vec3 m_position;
			glm::vec2 m_uv;

			Vertex(glm::vec3 pos, glm::vec2 uv) : m_position(pos), m_uv(uv) {}
		};

		Vertex vertices[4] = {
		Vertex(glm::vec3(-0.5f, -0.5f, 0), glm::vec2(0, 0)),
		Vertex(glm::vec3(-0.5f,  0.5f, 0), glm::vec2(0, 1)),
		Vertex(glm::vec3(0.5f,  0.5f, 0),  glm::vec2(1, 1)),
		Vertex(glm::vec3(0.5f, -0.5f, 0),  glm::vec2(1, 0))
		};

		uint32_t indices[] = { 0, 1, 2, 0, 2, 3 };

		VertexBufferLayout layout = {
			{VertexAttributeType::FLOAT3, "vsPos", 0},
			{VertexAttributeType::FLOAT2, "vsUv", 0},
		};

		m_ibo = std::make_shared<IndexBuffer>((byte*)indices, NUMOF(indices) * sizeof(uint32_t));
		m_vbo = std::make_shared<VertexBuffer>((byte*)vertices, NUMOF(vertices) * sizeof(Vertex));
		m_vao = std::make_shared<VertexArray>(layout);
		m_vao->addBuffer(m_vbo);
		m_vao->validate();
	}

	RenderPipeline::~RenderPipeline() {
	}

	void RenderPipeline::createRenderQueue() {}

	void RenderPipeline::processRenderQueue() {
		//while (!renderqueue_.empty()) {
		//	RenderCommand command = renderqueue_.front();
		//	renderqueue_.pop();
		//	// Process the render command
		//	switch (command.type) {
		//		case RenderCommand::Type::DRAW:
		//			// Draw the object
		//			glDrawArrays(command.mode, command.first, command.count);
		//			break;
		//		case RenderCommand::Type::CLEAR:
		//			// Clear the screen
		//			glClear(command.mask);
		//			break;
		//	}
		//}
	}

	void RenderPipeline::render() {
		Renderer::beginRenderPass(m_mainPass);
		m_mainPass->descriptor().shader->bind();
		m_vao->bind();
		m_ibo->bind();
		Renderer::drawIndexed(6, PrimitiveType::TRIANGLES, true);
		Renderer::endRenderPass();

		FrameBufferManager::bindDefaultFBO();
	}
}