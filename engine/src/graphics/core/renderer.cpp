#include "eepch.h"
#include "renderer.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "util/GLUtils.h"
#include "renderSyncManager.h"

namespace emerald {
	static Ref<RenderPass> s_activeRenderPass;
	static RenderSyncManager s_renderSyncManager;

	void Renderer::acquireRenderBuffer() {
		s_renderSyncManager.acquireRenderBuffer();
	}

	void Renderer::waitForBufferAvailability() {
		s_renderSyncManager.waitForBufferAvailability();
	}

	void Renderer::submitBufferForRendering() {
		s_renderSyncManager.submitBufferForRendering();
	}

	void Renderer::beginRenderPass(const Ref<RenderPass>& renderPass) {
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

	void Renderer::submit(RenderCommand command) {
		s_renderSyncManager.submit(command);
	}

	void Renderer::submitFromAnyThread(RenderCommand command) {
		s_renderSyncManager.submitFromAnyThread(command);
	}

	void Renderer::drawIndexed(uint32_t count, PrimitiveType type) {
		Renderer::submit([count, type] {
			GL(glDrawElements((uint32_t)type, count, GL_UNSIGNED_INT, nullptr));
		});
	}
}