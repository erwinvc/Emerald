#include "eepch.h"
#include "core/application/application.h"
#include "graphics/buffers/framebuffer.h"
#include "graphics/render/renderPass.h"
#include "renderer.h"
#include "renderSyncManager.h"
#include "utils/threading/threadManager.h"
#include "utils/misc/GLUtils.h"
#include "imguiProfiler/Profiler.h"
namespace emerald {
	static Ref<RenderPass> s_activeRenderPass;

	/*
	void Renderer::acquireRenderBuffer() {
		ASSERT(ThreadManager::isThread(ThreadType::RENDER), "Renderer::acquireRenderBuffer should be called on the render thread");
		s_renderSyncManager.acquireRenderBuffer();
	}

	void Renderer::waitForBufferAvailability() {
		ASSERT(ThreadManager::isThread(ThreadType::LOGIC), "Renderer::waitForBufferAvailability should be called on the logic thread");
		s_renderSyncManager.waitForBufferAvailability();
	}

	void Renderer::submitBufferForRendering() {
		ASSERT(ThreadManager::isThread(ThreadType::LOGIC), "Renderer::submitBufferForRendering should be called on the logic thread");
		s_renderSyncManager.submitBufferForRendering();
	}

	void Renderer::flushRenderCommands() {
		//Twice, because we use three buffers
		submitBufferForRendering();
		waitForBufferAvailability();
		submitBufferForRendering();
		waitForBufferAvailability();
	}
	*/
	void Renderer::beginRenderPass(const Ref<RenderPass>& renderPass) {
		s_activeRenderPass = renderPass;
		s_activeRenderPass->bind();
	}

	void Renderer::endRenderPass() {
		s_activeRenderPass = nullptr;
	}

	void Renderer::executeCommandBuffer() {
		s_renderSyncManager.executeRenderBuffer();
	}

	void Renderer::drawIndexed(uint32_t count, PrimitiveType type) {
		PROFILE_GPU_BEGIN("Draw Indexed");
		GL(glDrawElements((uint32_t)type, count, GL_UNSIGNED_INT, nullptr));
		PROFILE_GPU_END();
	}
}
