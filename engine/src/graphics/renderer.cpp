#include "eepch.h"
#include "renderer.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "util/GLUtils.h"
#include "../glError.h"
#include "renderSyncManager.h"

namespace emerald {
	static Ref<RenderPass> s_activeRenderPass;
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

	void Renderer::beginRenderPass(Ref<RenderPass> renderPass) {
		s_activeRenderPass = renderPass;
		s_activeRenderPass->bind();
		s_activeRenderPass->clear();
	}

	void Renderer::endRenderPass() {
		s_activeRenderPass = nullptr;
	}

	void Renderer::executeCommandBuffer() {
		s_renderSyncManager.executeRenderBuffer();
	}

	void Renderer::flushCommandBufferOnThisThread() {
		waitForBufferAvailability();
		submitBufferForRendering();
		acquireRenderBuffer();
		executeCommandBuffer();
	}

	void Renderer::submit(Command command) {
		s_renderSyncManager.submit(command);
	}

	void Renderer::submitFromAnyThread(Command command) {
		s_renderSyncManager.submitFromAnyThread(command);
	}

	void Renderer::drawIndexed(uint32_t count, PrimitiveType type, bool depthTest) {
		Renderer::submit([count, type, depthTest] {
			if (!depthTest)
				GL(glDisable(GL_DEPTH_TEST));

			GL(glDrawElements((uint32_t)type, count, GL_UNSIGNED_INT, nullptr));

			if (!depthTest)
				GL(glEnable(GL_DEPTH_TEST));
		});
	}
}