#pragma once
#include "eepch.h"
#include "renderPass.h"
#include "graphics/buffers/frameBuffer.h"

namespace emerald {
	RenderPass::RenderPass(RenderPassDesc desc) : m_desc(desc) {
	}

	RenderPass::~RenderPass() {
	}

	void RenderPass::clear() {
		m_desc.frameBuffer->clear(m_desc.clearMask);
	}

	void RenderPass::bind() {
		m_desc.frameBuffer->bind();
	}
}