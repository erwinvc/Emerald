#include "eepch.h"
#include "framebuffer.h"
#include "util/GLUtils.h"
#include "application.h"

namespace emerald {
	static const uint32_t drawBuffers[16] = {
	GL_COLOR_ATTACHMENT0,
	GL_COLOR_ATTACHMENT1,
	GL_COLOR_ATTACHMENT2,
	GL_COLOR_ATTACHMENT3,
	GL_COLOR_ATTACHMENT4,
	GL_COLOR_ATTACHMENT5,
	GL_COLOR_ATTACHMENT6,
	GL_COLOR_ATTACHMENT7,
	GL_COLOR_ATTACHMENT8,
	GL_COLOR_ATTACHMENT9,
	GL_COLOR_ATTACHMENT10,
	GL_COLOR_ATTACHMENT11,
	GL_COLOR_ATTACHMENT12,
	GL_COLOR_ATTACHMENT13,
	GL_COLOR_ATTACHMENT14,
	GL_COLOR_ATTACHMENT15
	};

	FrameBuffer::FrameBuffer(FramebufferDesc desc) : m_desc(desc) {

		uint32_t width = m_desc.width;
		uint32_t height = m_desc.height;
		if (width == 0) {
			width = App->getWidth();
			height = App->getHeight();
		}

		glGenFramebuffers(1, &m_handle);
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		GLUtils::glClearColor(m_desc.clearColor);

		for (auto& attachmentDesc : m_desc.attachments) {
			if (!GLUtils::isDepthFormat(attachmentDesc.format))
				m_textures.emplace_back(addBuffer(attachmentDesc.name, attachmentDesc.format, FBOAttachment::COLOR));
			else
				m_depthTexture = addBuffer(attachmentDesc.name, attachmentDesc.format, FBOAttachment::DEPTH);
		}

		resize(width, height, true);
	}

	std::shared_ptr<FrameBuffer> FrameBuffer::create(FramebufferDesc desc) {
		auto fbo = std::shared_ptr<FrameBuffer>(new FrameBuffer(desc)); 
		FrameBufferManager::add(fbo);
		return fbo;
	}


	FrameBuffer::~FrameBuffer() {
		glDeleteFramebuffers(1, &m_handle);
	}

	void FrameBuffer::resize(uint32_t width, uint32_t height, bool forceRecreate) {
		if (!forceRecreate && (getWidth() == width && getHeight() == height)) return;

		if (m_desc.scale == FBOScale::STATIC) {
			m_desc.width = width;
			m_desc.height = height;
		}

		for (auto& texture : m_textures) {
			texture->resize(m_desc.width, m_desc.height);
		}

		checkStatus();
	}


	void FrameBuffer::setScale(FBOScale scale) {
		if (m_desc.scale == scale) return;
		m_desc.scale = scale;
		resize(m_desc.width, m_desc.height, true);
	}

	float FrameBuffer::fboScaleToFloat(FBOScale scale) const {
		switch (scale) {
			case FBOScale::FULL: return 1.0f;
			case FBOScale::HALF: return 0.5f;
			case FBOScale::QUARTER: return 0.25f;
			case FBOScale::ONEEIGHTH: return 0.125f;
		}
		return 1.0f;
	}

	bool FrameBuffer::checkStatus() {
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			Log::error("Framebuffer {} failed: {}", m_desc.name.c_str(), GLUtils::getFBOStatus(status));
			return false;
		} else return true;
	}

	std::shared_ptr<Texture> FrameBuffer::addBuffer(const std::string& name, TextureFormat format, FBOAttachment type) {
		TextureDesc desc;
		desc.m_format = format;
		desc.m_filter = LINEAR;
		desc.m_wrap = REPEAT;
		desc.m_hasMipmaps = false;

		std::shared_ptr<Texture> texture = std::make_shared<Texture>(desc, m_desc.width, m_desc.height);
		texture->invalidate();
		//m_textureNames.push_back(name);

		bind();

		if (type == FBOAttachment::COLOR) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorAttachments, GL_TEXTURE_2D, texture->handle(), 0);
			glDrawBuffers(++m_colorAttachments, drawBuffers);
		} else if (type == FBOAttachment::DEPTH) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture->handle(), 0);
		} else if (type == FBOAttachment::STENCIL) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->handle(), 0);
		} else if (type == FBOAttachment::DEPTHSTENCIL) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture->handle(), 0);
		}

		checkStatus();

		return texture;
	}

	void FrameBuffer::blit(FrameBuffer* targetFBO) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
		uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
		uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
		glBlitFramebuffer(0, 0, m_desc.width, m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::blitColorOnly(FrameBuffer* targetFBO) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
		uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
		uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
		glBlitFramebuffer(0, 0, m_desc.width, m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::blitDepthOnly(FrameBuffer* targetFBO) const {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
		uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
		uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
		glBlitFramebuffer(0, 0, m_desc.width, m_desc.height, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}

	void FrameBuffer::bind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
		glViewport(0, 0, m_desc.width, m_desc.height);
	}
	void FrameBuffer::unbind() const {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	void FrameBuffer::clear() const {
		GLUtils::glClearColor(m_desc.clearColor);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::clearDepthOnly() const {
		glClear(GL_DEPTH_BUFFER_BIT);
	}

	void FrameBuffer::clearColorOnly() const {
		GLUtils::glClearColor(m_desc.clearColor);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void FrameBuffer::clearStencilOnly() const {
		glClear(GL_STENCIL_BUFFER_BIT);
	}

	void FrameBuffer::setDrawAndReadBuffersToNone() const {
		bind();
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		unbind();
	}

	namespace FrameBufferManager {
		void add(const std::shared_ptr<FrameBuffer>& fbo) {
			m_frameBuffers.push_back(fbo);
		}

		void bindDefaultFBO() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0, 0, App->getWidth(), App->getHeight());
		}

		void onResize(uint32_t width, uint32_t height) {
			auto it = m_frameBuffers.begin();
			while (it != m_frameBuffers.end()) {
				if (auto framebuffer = it->lock()) { 
					framebuffer->resize(width, height, false);
					++it; 
				} else {
					it = m_frameBuffers.erase(it);
				}
			}
		}

		void shutdown() {
			m_frameBuffers.clear();
		}
	}
}