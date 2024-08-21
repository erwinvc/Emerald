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
#include "imguiProfiler/Profiler.h"
#include "input/keyboard.h"

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
		m_mainPass = Ref<RenderPass>::create(mainPassDesc);

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

		m_ibo = Ref<IndexBuffer>::create((byte*)indices, (uint32_t)(NUMOF(indices) * sizeof(uint32_t)));
		m_vbo = Ref<VertexBuffer>::create((byte*)vertices, (uint32_t)(NUMOF(vertices) * sizeof(Vertex)));
		m_vao = Ref<VertexArray>::create(layout);
		m_vao->addBuffer(m_vbo);
		m_vao->validate();

		Renderer::flushCommandBufferOnThisThread();

		m_material = Ref<Material>::create("Geometry", mainPassDesc.shader);
		//m_material->Set("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		glm::vec4 colors[2] = { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) };
		m_material->SetArray("color", colors, 2);
		m_material->Set("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0);
		m_material->Set("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1);
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

	int index = 0;
	void RenderPipeline::render() {
		if (Keyboard::keyJustDown(Key::R)) {
			index++;
			if(index > 1) index = 0;
			m_material->Set("colorIndex", index);
		}

		Renderer::submit([] {PROFILE_RENDER_BEGIN("Pipeline"); });
		Renderer::beginRenderPass(m_mainPass);
		m_material->updateForRendering();
		m_vao->bind();
		m_ibo->bind();

		Renderer::drawIndexed(6, PrimitiveType::TRIANGLES, true);
		Renderer::endRenderPass();

		FrameBufferManager::bindDefaultFBO();
		Renderer::submit([] {PROFILE_RENDER_END(); });
	}

	Ref<Texture> RenderPipeline::getFinalTexture() {
		return m_mainPass->descriptor().frameBuffer->getTextures()[0];
	}

}