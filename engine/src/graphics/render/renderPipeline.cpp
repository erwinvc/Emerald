#include "eepch.h"
#include "core/application/application.h"
#include "engine/assets/loaders/modelLoader.h"
#include "engine/ecs/components/meshRendererComponent.h"
#include "engine/ecs/components/transformComponent.h"
#include "engine/input/keyboard.h"
#include "engine/scene/sceneManager.h"
#include "graphics/buffers/framebuffer.h"
#include "graphics/buffers/indexBuffer.h"
#include "graphics/core/renderer.h"
#include "graphics/shaders/shader.h"
#include "imguiProfiler/Profiler.h"
#include "renderPass.h"
#include "renderPipeline.h"
#include "../../editor/src/core/editor.h"
#include "engine/ecs/core/ECSManager.h"

namespace emerald {
	RenderPipeline::RenderPipeline() {
		FramebufferDesc mainfbDesc;
		mainfbDesc.width = App->getWidth() / 8;
		mainfbDesc.height = App->getHeight() / 8;
		mainfbDesc.samples = MSAA::X4;
		mainfbDesc.attachments = { {"Main", TextureFormat::RGBA8}, {"Depth", TextureFormat::DEPTH24STENCIL8} };
		mainfbDesc.clearColor = { 0.5f, 0.7f, 1.0f, 1.0f };
		mainfbDesc.name = "MainFB";

		// Resolve
		FramebufferDesc resolveFbDesc;
		resolveFbDesc.width = App->getWidth();
		resolveFbDesc.height = App->getHeight();
		resolveFbDesc.attachments = { {"Resolved", TextureFormat::RGBA8} };
		resolveFbDesc.clearColor = { 0.5f, 0.7f, 1.0f, 1.0f };
		resolveFbDesc.name = "ResolveFB";

		m_resolveFramebuffer = FrameBuffer::create(resolveFbDesc);

		// Main
		RenderPassDesc mainPassDesc;
		mainPassDesc.frameBuffer = FrameBuffer::create(mainfbDesc);
		shader = Ref<Shader>::create("Geometry", "res/shaders/geometry");
		mainPassDesc.shader = shader;
		m_mainPass = Ref<RenderPass>::create(mainPassDesc);

		// Shadow
		FramebufferDesc shadowFbDesc;
		shadowFbDesc.width = 2048;
		shadowFbDesc.height = 2048;
		shadowFbDesc.scale = FBOScale::STATIC;
		shadowFbDesc.samples = MSAA::NONE;
		shadowFbDesc.attachments = {
			{"VSM", TextureFormat::RG32F},
			{"Depth", TextureFormat::DEPTH24STENCIL8}
		};
		shadowFbDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		shadowFbDesc.name = "ShadowFB";

		m_shadowFramebuffer = FrameBuffer::create(shadowFbDesc);

		m_shadowShader = Ref<Shader>::create("Shadow", "res/shaders/shadow");

		RenderPassDesc shadowPassDesc;
		shadowPassDesc.frameBuffer = m_shadowFramebuffer;
		shadowPassDesc.shader = m_shadowShader;
		m_shadowPass = Ref<RenderPass>::create(shadowPassDesc);

		Renderer::flushRenderCommands();

		m_shadowMaterial = Ref<Material>::create("Shadow", m_shadowShader);

		// Blur
		FramebufferDesc blurFbDesc;
		blurFbDesc.width = 2048;
		blurFbDesc.height = 2048;
		blurFbDesc.samples = MSAA::NONE;
		blurFbDesc.attachments = {
			{"BlurColor", TextureFormat::RG32F}
		};
		blurFbDesc.clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		blurFbDesc.name = "BlurFB_Horizontal";

		m_blurFBOHorizontal = FrameBuffer::create(blurFbDesc);

		blurFbDesc.name = "BlurFB_Vertical";
		m_blurFBOVertical = FrameBuffer::create(blurFbDesc);

		updateLightMatrices();
	}

	RenderPipeline::~RenderPipeline() {
	}

	void RenderPipeline::updateLightMatrices() {
		// Update light view matrix
		m_lightView = glm::lookAt(
			m_lightPosition,
			m_lightTarget,
			glm::vec3(0.0f, 1.0f, 0.0f)
		);

		// Update light projection matrix
		m_lightProjection = glm::ortho(
			-m_orthoSize, m_orthoSize,
			-m_orthoSize, m_orthoSize,
			m_zNear, m_zFar
		);
	}
	void RenderPipeline::onImGuiRender() {

		//ImGui::Begin("Debug ShadowMap", nullptr);
		//ImGui::End();
		//
		//ImGui::Begin("Light Controls");
		//
		//if (ImGui::DragFloat3("Light Position", &m_lightPosition[0], 0.1f)) {
		//	updateLightMatrices();
		//}
		//if (ImGui::DragFloat3("Light Target", &m_lightTarget[0], 0.1f)) {
		//	updateLightMatrices();
		//}
		//if (ImGui::DragFloat("Shadow Area Size", &m_orthoSize, 0.5f, 1.0f, 500.0f)) {
		//	updateLightMatrices();
		//}
		//if (ImGui::DragFloat("zNear", &m_zNear, 0.1f, 0.001f, 100.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) {
		//	updateLightMatrices();
		//}
		//if (ImGui::DragFloat("zFar", &m_zFar, 0.1f, 1.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic)) {
		//	updateLightMatrices();
		//}
		//if (ImGui::TreeNode("Debug Info")) {
		//	glm::vec3 lightDir = glm::normalize(m_lightTarget - m_lightPosition);
		//	ImGui::Text("Light Direction: %.2f, %.2f, %.2f", lightDir.x, lightDir.y, lightDir.z);
		//	ImGui::TreePop();
		//}
		//
		//ImGui::End();
		//
		//if (ImGui::Begin("Debug ShadowMap", nullptr)) {
		//	float sizeX = 512;
		//	float sizeY = 512;
		//
		//	ImTextureID texID = (ImTextureID)(intptr_t)m_shadowFramebuffer->getTextures()[0]->handle();
		//	ImGui::Image(texID, ImVec2(sizeX, sizeY), ImVec2(0, 1), ImVec2(1, 0));
		//}
		//ImGui::End();
	}
	void RenderPipeline::render() {
		//return;

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

		SceneManager::getActiveScene()->updateAllTransforms();

		{
			Renderer::submit([] {
				PROFILE_RENDER_BEGIN("ShadowPass");
			});
			Renderer::beginRenderPass(m_shadowPass);

			Renderer::submit([] {
				glEnable(GL_DEPTH_TEST);
				glClear(GL_DEPTH_BUFFER_BIT);
				glDisable(GL_CULL_FACE);
				//glEnable(GL_CULL_FACE);
				//glCullFace(GL_FRONT); // optional to reduce acne
			});

			m_shadowMaterial->updateForRendering();

			glm::mat4 lightSpaceMatrix = m_lightProjection * m_lightView;
			m_shadowMaterial->set("_LightSpaceMatrix", lightSpaceMatrix);

			auto view = EntityComponentSystem::View<MeshRendererComponent, TransformComponent>();
			for (auto [meshRenderer, transform] : view) {
				if (!meshRenderer->m_mesh) continue;
  				glm::mat4 model = transform->getGlobalTransform();
				m_shadowMaterial->set("_ModelMatrix", model);

				meshRenderer->m_mesh->bind();
				Renderer::drawIndexed(meshRenderer->m_mesh->getIBO()->getCount(), PrimitiveType::TRIANGLES);
			}

			Renderer::endRenderPass();
			Renderer::submit([] {
				PROFILE_RENDER_END();
			});
		}

		Renderer::submit([] {PROFILE_RENDER_BEGIN("Pipeline"); });
		Renderer::beginRenderPass(m_mainPass);

		Renderer::submit([] {
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			glFrontFace(GL_CCW);
			GL(glEnable(GL_DEPTH_TEST));
		});

		//multithread this?
		auto view = EntityComponentSystem::View<MeshRendererComponent, TransformComponent>();
		for (auto [meshRenderer, transform] : view) {
			if (!meshRenderer->m_mesh) continue;

			Ref<Material> mat = meshRenderer->m_mesh->getMaterial();
			mat->set("_ViewMatrix", Editor->getEditorCamera()->getViewMatrix());
			mat->set("_ProjectionMatrix", Editor->getEditorCamera()->getProjectionMatrix());
			mat->set("_ModelMatrix", transform->getGlobalTransform());
			mat->set("_LightSpaceMatrix", m_lightProjection * m_lightView);
			mat->set("_ViewPosition", Editor->getEditorCamera()->getPosition());
			glm::vec3 lightDir = glm::normalize(m_lightPosition - m_lightTarget);
			mat->set("_LightPosition", m_lightPosition);
			mat->set("_LightDirection", lightDir);
			mat->setTexture("_VSM", m_shadowFramebuffer->getTextures()[0]);
			mat->updateForRendering();
			meshRenderer->m_mesh->bind();
			Renderer::drawIndexed(meshRenderer->m_mesh->getIBO()->getCount(), PrimitiveType::TRIANGLES);
		}

		Renderer::endRenderPass();

		Renderer::submit([] {PROFILE_RENDER_BEGIN("Blit"); });
		m_mainPass->descriptor().frameBuffer->blitColorOnly(m_resolveFramebuffer);
		Renderer::submit([] {PROFILE_RENDER_END(); });

		FrameBufferManager::bindDefaultFBO();
		Renderer::submit([] {PROFILE_RENDER_END(); });
	}

	const Ref<Texture>& RenderPipeline::getFinalTexture() {
		return m_resolveFramebuffer->getTextures()[0];
	}
}