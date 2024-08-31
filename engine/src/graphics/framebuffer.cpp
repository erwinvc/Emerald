#include "eepch.h"
#include "framebuffer.h"
#include "util/GLUtils.h"
#include "application.h"
#include "renderer.h"
#include "glError.h"

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
		Renderer::submit([instance = Ref<FrameBuffer>(this)]() mutable {
			uint32_t width = instance->m_desc.width;
			uint32_t height = instance->m_desc.height;
			if (width == 0) {
				width = App->getWidth();
				height = App->getHeight();
			}

			GL(glGenFramebuffers(1, &instance->m_handle));
			GL(glBindFramebuffer(GL_FRAMEBUFFER, instance->m_handle));

			GLUtils::glClearColor(instance->m_desc.clearColor);

			for (auto& attachmentDesc : instance->m_desc.attachments) {
				Ref<Texture> texture = instance->addBuffer(attachmentDesc.name, attachmentDesc.format);
				if (!GLUtils::isDepthFormat(attachmentDesc.format)) instance->m_textures.emplace_back(texture);
				else instance->m_depthTexture = texture;
			}

			instance->resize(width, height, true);

			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		});
	}

	Ref<FrameBuffer> FrameBuffer::create(FramebufferDesc desc) {
		Ref<FrameBuffer> fbo{ new FrameBuffer(desc) };
		FrameBufferManager::add(fbo);
		return fbo;
	}


	FrameBuffer::~FrameBuffer() {
		Renderer::submitFromAnyThread([handle = m_handle] {
			GL(glDeleteFramebuffers(1, &handle));
		});
	}

	void FrameBuffer::resize(uint32_t width, uint32_t height, bool forceRecreate) {
		if (!forceRecreate && (getWidth() == width && getHeight() == height)) return;

		if (m_desc.scale == FBOScale::STATIC) {
			m_desc.width = width;
			m_desc.height = height;
		} else {
			m_desc.width = (uint32_t)(width * fboScaleToFloat(m_desc.scale));
			m_desc.height = (uint32_t)(height * fboScaleToFloat(m_desc.scale));
		}

		for (auto& texture : m_textures) {
			texture->resize(m_desc.width, m_desc.height);
		}

		if(m_depthTexture != nullptr) m_depthTexture->resize(m_desc.width, m_desc.height);

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
		GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
		if (status != GL_FRAMEBUFFER_COMPLETE) {
			Log::error("Framebuffer {} failed: {}", m_desc.name.c_str(), GLUtils::getFBOStatus(status));
			return false;
		} else return true;
	}

	Ref<Texture> FrameBuffer::addBuffer(const std::string& name, TextureFormat format) {
		TextureDesc desc;
		desc.format = format;
		desc.filter = LINEAR;
		desc.wrap = REPEAT;
		desc.hasMipmaps = false;
		desc.isImmutable = false;

		Ref<Texture> texture = Ref<Texture>::create(desc, m_desc.width, m_desc.height);
		texture->invalidate();
		//m_textureNames.push_back(name);

		glBindFramebuffer(GL_FRAMEBUFFER, m_handle);

		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, desc.textureFormatToAttachmentType(m_colorAttachments), GL_TEXTURE_2D, texture->handle(), 0));
		if(desc.isColorAttachmentType()) {
			GL(glDrawBuffers(++m_colorAttachments, drawBuffers));
		}
		
		checkStatus();

		return texture;
	}

	/*
	void FrameBuffer::blit(Ref<const FrameBuffer> targetFBO) const {
		Ref<const FrameBuffer> instance = this;
		Renderer::submit([instance, targetFBO] {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, instance->m_handle);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitFramebuffer(0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		});
	}

	void FrameBuffer::blitColorOnly(Ref<const FrameBuffer> targetFBO) const {
		Ref<const FrameBuffer> instance = this;
		Renderer::submit([instance, targetFBO] {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, instance->m_handle);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitFramebuffer(0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		});
	}

	void FrameBuffer::blitDepthOnly(Ref<const FrameBuffer> targetFBO) const {
		Ref<const FrameBuffer> instance = this;
		Renderer::submit([instance, targetFBO] {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, instance->m_handle);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFBO ? targetFBO->handle() : 0);
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitFramebuffer(0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		});
	}
	*/
	void FrameBuffer::bind() const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this)] {
			GL(glBindFramebuffer(GL_FRAMEBUFFER, instance->m_handle));
			GL(glViewport(0, 0, instance->m_desc.width, instance->m_desc.height));
		});
	}
	void FrameBuffer::unbind() const {
		Renderer::submit([] {
			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		});
	}
	void FrameBuffer::clear() const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this)] {
			GLUtils::glClearColor(instance->m_desc.clearColor);
			GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		});
	}

	void FrameBuffer::clearDepthOnly() const {
		Renderer::submit([] {
			GL(glClear(GL_DEPTH_BUFFER_BIT));
		});
	}

	void FrameBuffer::clearColorOnly() const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this)] {
			GLUtils::glClearColor(instance->m_desc.clearColor);
			GL(glClear(GL_COLOR_BUFFER_BIT));
		});
	}

	void FrameBuffer::clearStencilOnly() const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this)] {
			GL(glClear(GL_STENCIL_BUFFER_BIT));
		});
	}

	void FrameBuffer::setDrawAndReadBuffersToNone() const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this)] {
			GL(glDrawBuffer(GL_NONE));
			GL(glReadBuffer(GL_NONE));
		});
	}

	namespace FrameBufferManager {
		void add(Ref<FrameBuffer> fbo) {
			m_frameBuffers.push_back(fbo);
		}

		void bindDefaultFBO() {
			Renderer::submit([width = App->getWidth(), height = App->getHeight()] {
				GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
				GL(glViewport(0, 0, width, height));
			});
		}

		void onResize(uint32_t width, uint32_t height) {
			auto it = m_frameBuffers.begin();
			while (it != m_frameBuffers.end()) {
				if ((*it).getRefCount() > 1) {
					(*it)->resize(width, height, false);
					++it;
				} else {
					//it = m_frameBuffers.erase(it);
				}
			}
		}

		void shutdown() {
			m_frameBuffers.clear();
		}
	}
}