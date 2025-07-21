#include "eepch.h"
#include "picking.h"

namespace emerald {
	Picking::Picking() {
		FramebufferDesc desc;
		desc.samples = MSAA::NONE;
		desc.attachments = { {"ID", TextureFormat::R32UI } };
		desc.clearColor = { 0.0f, 0.0f, 0.0f, 0.0f };
		desc.name = "Picking";
		m_fbo = FrameBuffer::create(desc);

		m_shader = Ref<Shader>::create("ID", "res/shaders/id");
		RenderPassDesc passDesc;
		passDesc.name = "Picking Pass";
		passDesc.frameBuffer = m_fbo;
		passDesc.shader = m_shader;
		m_renderPass = Ref< RenderPass>::create(passDesc);
	}


}