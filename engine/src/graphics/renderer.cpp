#include "eepch.h"
#include "renderer.h"
#include "renderPass.h"
#include "framebuffer.h"
#include "application.h"
#include "util/GLUtils.h"
#include "../glError.h"

namespace emerald {
	static RendererData s_data;

	void Renderer::beginRenderPass(std::shared_ptr<RenderPass> renderPass) {
		s_data.m_activeRenderPass = renderPass;
		s_data.m_activeRenderPass->bind();
		s_data.m_activeRenderPass->clear();
	}

	void Renderer::endRenderPass() {
		s_data.m_activeRenderPass = nullptr;
	}

	void Renderer::bindDefaultFBO() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, App->getWidth(), App->getHeight());
	}

	void Renderer::executeCommandBuffer() {
		s_data.m_commandBuffer.executeCommands();
	}

	void Renderer::submit(Command command) {
		s_data.m_commandBuffer.pushCommand(command);
	}

	void Renderer::drawIndexed(uint32_t count, PrimitiveType type, bool depthTest) {
		if (!depthTest)
			glDisable(GL_DEPTH_TEST);

		glDrawElements((uint32_t)type, count, GL_UNSIGNED_INT, nullptr);

		if (!depthTest)
			glEnable(GL_DEPTH_TEST);
	}

}