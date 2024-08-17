#include "eepch.h"
#include "renderer.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "util/GLUtils.h"
#include "../glError.h"
#include "renderSyncManager.h"

namespace emerald {
	static std::shared_ptr<RenderPass> s_activeRenderPass;
	static RenderSyncManager s_renderSyncManager;

	void Renderer::setTempBuffer() {
		s_renderSyncManager.SetTempBuffer();
	}
		void Renderer::acquireRenderBuffer() {
		s_renderSyncManager.acquireRenderBuffer();
	}

	void Renderer::waitForBufferAvailability() {
		s_renderSyncManager.waitForBufferAvailability();
	}

	void Renderer::submitBufferForRendering() {
		s_renderSyncManager.submitBufferForRendering();
	}

	void Renderer::beginRenderPass(std::shared_ptr<RenderPass> renderPass) {
		s_activeRenderPass = renderPass;
		s_activeRenderPass->bind();
		s_activeRenderPass->clear();
	}

	void Renderer::endRenderPass() {
		s_activeRenderPass = nullptr;
	}

	void Renderer::bindDefaultFBO() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, App->getWidth(), App->getHeight());
	}

	void Renderer::executeCommandBuffer() {
		s_renderSyncManager.executeRenderBuffer();
	}

	void Renderer::submit(Command command) {
		s_renderSyncManager.submit(command);
	}

	void Renderer::drawIndexed(uint32_t count, PrimitiveType type, bool depthTest) {
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);

		glDrawElements((uint32_t)type, count, GL_UNSIGNED_INT, nullptr);

		if (!depthTest)
			glEnable(GL_DEPTH_TEST);
	}

}