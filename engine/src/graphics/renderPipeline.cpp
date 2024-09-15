#include "eepch.h"
#include "renderPipeline.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "renderer.h"
#include "graphics/shaders/shader.h"
#include "buffers/indexBuffer.h"
#include "vertexArray.h"
#include "imguiProfiler/Profiler.h"
#include "input/keyboard.h"
#include "../../editor/src/editor.h"
#include "engineIcon.h"
#include "assets/loaders/modelLoader.h"
#include "../scene/sceneManager.h"
#include "ecs/components/transformComponent.h"
#include "ecs/components/meshRendererComponent.h"

namespace emerald {
	RenderPipeline::RenderPipeline() {
		FramebufferDesc mainfbDesc;
		mainfbDesc.width = App->getWidth() / 8;
		mainfbDesc.height = App->getHeight() / 8;
		mainfbDesc.samples = MSAA::X4;
		mainfbDesc.attachments = { {"Main", TextureFormat::RGBA8F}, {"Depth", TextureFormat::DEPTH24STENCIL8} };
		mainfbDesc.clearColor = { 0.5f, 0.7f, 1.0f, 1.0f };
		mainfbDesc.name = "MainFB";

		FramebufferDesc resolveFbDesc;
		resolveFbDesc.width = App->getWidth() / 8;
		resolveFbDesc.height = App->getHeight() / 8;
		resolveFbDesc.attachments = { {"Resolved", TextureFormat::RGBA8F} };
		resolveFbDesc.clearColor = { 0.5f, 0.7f, 1.0f, 1.0f };
		resolveFbDesc.name = "ResolveFB";

		m_resolveFramebuffer = FrameBuffer::create(resolveFbDesc);

		RenderPassDesc mainPassDesc;
		mainPassDesc.frameBuffer = FrameBuffer::create(mainfbDesc);
		mainPassDesc.shader = Ref<Shader>::create("Geometry", "res/shaders/geometry");
		m_mainPass = Ref<RenderPass>::create(mainPassDesc);

		struct Vertex {
			glm::vec3 m_position;
			glm::vec2 m_uv;

			Vertex(glm::vec3 pos, glm::vec2 uv) : m_position(pos), m_uv(uv) {}
		};

		Vertex vertices[24] = {
			// Front face
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0, 1)),

			// Back face
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0, 1)),

			// Left face
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0, 1)),

			// Right face
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(0, 1)),

			// Top face
			Vertex(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0, 1)),

			// Bottom face
			Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0, 0)),
			Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1, 0)),
			Vertex(glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1, 1)),
			Vertex(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0, 1))
		};

		uint32_t indices[] = {
			// Front face
			0, 1, 2, 0, 2, 3,

			// Back face
			4, 5, 6, 4, 6, 7,

			// Left face
			8, 9, 10, 8, 10, 11,

			// Right face
			12, 13, 14, 12, 14, 15,

			// Top face
			16, 17, 18, 16, 18, 19,

			// Bottom face
			20, 21, 22, 20, 22, 23
		};

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

		//glm::vec4 colors[2] = { glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) };
		//m_material->SetArray("color", colors, 2);
		m_material->set("color", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 0);
		m_material->set("color", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1);
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
		if (Keyboard::keyJustDown(Key::KEY_1)) {
			m_mainPass->descriptor().frameBuffer->setMSAA(MSAA::NONE);
		}
		if (Keyboard::keyJustDown(Key::KEY_2)) {
			m_mainPass->descriptor().frameBuffer->setMSAA(MSAA::X2);
		}
		if (Keyboard::keyJustDown(Key::KEY_3)) {
			m_mainPass->descriptor().frameBuffer->setMSAA(MSAA::X4);
		}
		if (Keyboard::keyJustDown(Key::KEY_4)) {
			m_mainPass->descriptor().frameBuffer->setMSAA(MSAA::X8);
		}
		if (Keyboard::keyJustDown(Key::KEY_5)) {
			m_mainPass->descriptor().frameBuffer->setMSAA(MSAA::X16);
		}
		if (Keyboard::keyJustDown(Key::R)) {
			index++;
			if (index > 1) index = 0;
			m_material->set("colorIndex", index);
		}

		Renderer::submit([] {PROFILE_RENDER_BEGIN("Pipeline"); });
		Renderer::beginRenderPass(m_mainPass);

		Renderer::submit([] {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			GL(glEnable(GL_DEPTH_TEST));
		});

		glm::mat4 modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 0.1f, 0.1f));

		m_material->set("viewMatrix", Editor->getEditorCamera()->getViewMatrix());
		m_material->set("projectionMatrix", Editor->getEditorCamera()->getProjectionMatrix());

		SceneManager::getActiveScene()->updateAllTransforms();

		for (auto& meshRenderer : SceneManager::getActiveScene()->getECS().getComponentArray<MeshRendererComponent>()) {
			auto* transform = SceneManager::getActiveScene()->getECS().getComponent<TransformComponent>(meshRenderer->m_entity);
			m_material->set("modelMatrix", transform->getGlobalTransform());
			m_material->updateForRendering();
			meshRenderer->m_mesh->bind();
			Renderer::drawIndexed(meshRenderer->m_mesh->getIBO()->getCount(), PrimitiveType::TRIANGLES);
		}

		Renderer::endRenderPass();

		m_mainPass->descriptor().frameBuffer->blitColorOnly(m_resolveFramebuffer);

		FrameBufferManager::bindDefaultFBO();
		Renderer::submit([] {PROFILE_RENDER_END(); });
	}

	const Ref<Texture>& RenderPipeline::getFinalTexture() {
		return m_resolveFramebuffer->getTextures()[0];
	}

}