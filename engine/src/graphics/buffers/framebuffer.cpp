#include "eepch.h"
#include "framebuffer.h"
#include "utils/misc/GLUtils.h"
#include "core/application/application.h"
#include "graphics/core/renderer.h"
#include "utils/threading/threadManager.h"

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

			instance->invalidateTextures();
			instance->attachTextures();
			//	instance->resize(width, height, true);

			GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		});
	}

	Ref<FrameBuffer> FrameBuffer::create(FramebufferDesc desc) {
		Ref<FrameBuffer> fbo{ new FrameBuffer(desc) };
		FrameBufferManager::add(fbo);
		return fbo;
	}

	void FrameBuffer::invalidateTextures() {
		ASSERT(ThreadManager::isThread(ThreadType::RENDER), "framebuffers should be invalidated on the render thread");
		m_textures.clear();
		m_depthTexture = nullptr;
		for (auto& attachmentDesc : m_desc.attachments) {
			TextureDesc desc;
			desc.name = m_desc.name + "_" + attachmentDesc.name;
			desc.format = attachmentDesc.format;
			desc.filter = TextureFilter::LINEAR;
			desc.wrap = TextureWrap::REPEAT;
			desc.hasMipmaps = false;
			desc.isImmutable = false;
			desc.samples = m_desc.samples;

			const Ref<Texture>& texture = Ref<Texture>::create(desc, m_desc.width, m_desc.height);
			texture->invalidate();

			Log::info("Creating {} texture for framebuffer: {} at {}x{}", attachmentDesc.name, m_desc.name, m_desc.width, m_desc.height);
			if (!GLUtils::isDepthFormat(attachmentDesc.format)) m_textures.emplace_back(texture);
			else m_depthTexture = texture;
		}
	}

	void FrameBuffer::attachTextures() {
		GL(glBindFramebuffer(GL_FRAMEBUFFER, m_handle));

#ifdef EE_DEBUG
		GL(glObjectLabel(GL_FRAMEBUFFER, m_handle, static_cast<GLsizei>(m_desc.name.size()), m_desc.name.c_str()));
#endif
		m_colorAttachments = 0;
		uint32_t attachmentIndex = 0;
		for (auto& texture : m_textures) {
			const TextureDesc& desc = texture->descriptor();
			GLenum attachmentType = desc.textureFormatToAttachmentType(attachmentIndex);

			GL(glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, texture->handle(), 0));

			if (desc.isColorAttachmentType()) {
				attachmentIndex++;
			}
		}

		if (m_depthTexture != nullptr) {
			const TextureDesc& desc = m_depthTexture->descriptor();
			GLenum attachmentType = desc.textureFormatToAttachmentType(0);

			GL(glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, m_depthTexture->handle(), 0));
		}

		if (attachmentIndex > 0) {
			GL(glDrawBuffers(attachmentIndex, drawBuffers));
		} else {
			GL(glDrawBuffer(GL_NONE));
		}

		checkStatus();

		GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	}

	FrameBuffer::~FrameBuffer() {
		Renderer::submitFromAnyThread([handle = m_handle] {
			GL(glDeleteFramebuffers(1, &handle));
		});
	}

	void FrameBuffer::resize(uint32_t width, uint32_t height, bool forceRecreate) {
		if (!forceRecreate && (getWidth() == width && getHeight() == height) || m_desc.scale == FBOScale::STATIC) return;

		m_desc.width = (uint32_t)(width * fboScaleToFloat(m_desc.scale));
		m_desc.height = (uint32_t)(height * fboScaleToFloat(m_desc.scale));

		if (m_desc.samples != MSAA::NONE) {
			// If we use multisampling we need to recreate all textures
			invalidateTextures();
			attachTextures();
		} else {
			// If we don't use multisampling we can simply resize the textures
			for (auto& texture : m_textures) {
				texture->resize(m_desc.width, m_desc.height);
			}

			if (m_depthTexture != nullptr) {
				m_depthTexture->resize(m_desc.width, m_desc.height);
			}
		}
	}

	void FrameBuffer::setScale(FBOScale scale) {
		if (m_desc.scale == scale) return;
		m_desc.scale = scale;

		Renderer::submit([instance = Ref<FrameBuffer>(this)]() mutable {
			instance->resize(instance->m_desc.width, instance->m_desc.height, true);
		});
	}

	void FrameBuffer::setMSAA(MSAA msaa) {
		if (m_desc.samples != msaa) {
			m_desc.samples = msaa;
			Renderer::submit([instance = Ref<FrameBuffer>(this)]() mutable {
				instance->invalidateTextures();
				instance->attachTextures();
			});
		}
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


	void FrameBuffer::blit(const Ref<FrameBuffer>& targetFBO) const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this), targetFBO = Ref<FrameBuffer>(targetFBO)] {
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitNamedFramebuffer(instance->m_handle, targetFBO ? targetFBO->handle() : 0, 0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		});
	}

	void FrameBuffer::blitColorOnly(const Ref<FrameBuffer>& targetFBO) const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this), targetFBO = Ref<FrameBuffer>(targetFBO)] {
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitNamedFramebuffer(instance->m_handle, targetFBO ? targetFBO->handle() : 0, 0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		});
	}

	void FrameBuffer::blitDepthOnly(const Ref<FrameBuffer>& targetFBO) const {
		Renderer::submit([instance = Ref<const FrameBuffer>(this), targetFBO = Ref<FrameBuffer>(targetFBO)] {
			uint32_t w = targetFBO ? targetFBO->getWidth() : App->getWidth();
			uint32_t h = targetFBO ? targetFBO->getHeight() : App->getHeight();
			glBlitNamedFramebuffer(instance->m_handle, targetFBO ? targetFBO->handle() : 0, 0, 0, instance->m_desc.width, instance->m_desc.height, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		});
	}

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
		void add(const Ref<FrameBuffer>& fbo) {
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