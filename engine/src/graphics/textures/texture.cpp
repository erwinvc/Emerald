#include "eepch.h"
#include "texture.h"
#include "graphics/textures/textureDesc.h"
#include "utils/misc/utils.h"
#include "utils/system/fileSystem.h"

#include "graphics/core/renderer.h"
#include "core/common/assrt.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "utils/threading/threadManager.h"

namespace emerald {
	Texture::Texture(TextureDesc desc, uint32_t width, uint32_t height, const byte* data, uint32_t dataSize)
		: m_desc(desc), m_width(width), m_height(height), m_dirty(true) {
		if (data) {
			m_buffer = Buffer<byte>::copy((byte*)data, dataSize);
		}
	}

	Texture::Texture(TextureDesc desc, uint32_t width, uint32_t height)
		: m_desc(desc), m_width(width), m_height(height), m_dirty(true) {
	}

	Texture::~Texture() {
		cleanup();
	}

	void Texture::cleanup() const {
		if (m_handle) {
			auto id = m_handle;
			Renderer::submitFromAnyThread([id] {
				GL(glDeleteTextures(1, &id));
			});
		}
	}

	void Texture::invalidate() {
		ASSERT(ThreadManager::isThread(ThreadType::RENDER), "textures should be invalidated on the render thread");

		if (m_handle) cleanup();

		GLenum target = m_desc.getTarget();

		GL(glCreateTextures(target, 1, &m_handle));
#ifdef EE_DEBUG
		GL(glObjectLabel(GL_TEXTURE, m_handle, static_cast<GLsizei>(m_desc.name.size()), m_desc.name.c_str()));
#endif
		if (m_desc.samples != MSAA::NONE) {
			// Multisampled texture
			GL(glTextureStorage2DMultisample(m_handle, m_desc.getSamples(), m_desc.getInternalFormat(), m_width, m_height, GL_TRUE));
		} else {
			// Regular texture
			m_mipmapCount = m_desc.hasMipmaps ? utils::calculateMipCount(m_width, m_height) : 1;

			if (m_desc.isImmutable) {
				// Use immutable texture storage
				GL(glTextureStorage2D(m_handle, m_mipmapCount, m_desc.getInternalFormat(), m_width, m_height));
			} else {
				// Use mutable texture storage
				GL(glBindTexture(target, m_handle));
				GL(glTexImage2D(target, 0, m_desc.getInternalFormat(), m_width, m_height, 0, m_desc.getImageFormat(), m_desc.getDataType(), nullptr));
				if (m_desc.hasMipmaps) GL(glGenerateMipmap(target));
			}

			if (m_buffer) {
				auto imageFormat = m_desc.getImageFormat();
				auto dataType = m_desc.getDataType();
				GL(glTextureSubImage2D(m_handle, 0, 0, 0, m_width, m_height, imageFormat, dataType, m_buffer.data()));
				if (m_desc.hasMipmaps) GL(glGenerateTextureMipmap(m_handle));
			}

			if (!m_desc.readWrite) m_buffer.clear();

			GL(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_R, m_desc.getWrap()));
			GL(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_S, m_desc.getWrap()));
			GL(glTextureParameteri(m_handle, GL_TEXTURE_WRAP_T, m_desc.getWrap()));
			GL(glTextureParameteri(m_handle, GL_TEXTURE_MIN_FILTER, m_desc.getFilter(GL_TEXTURE_MIN_FILTER, m_desc.hasMipmaps)));
			GL(glTextureParameteri(m_handle, GL_TEXTURE_MAG_FILTER, m_desc.getFilter(GL_TEXTURE_MAG_FILTER, m_desc.hasMipmaps)));

			if (m_desc.isDepthAttachmentType()) {
				glTextureParameteri(m_handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTextureParameteri(m_handle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			}

			m_dirty = false;
		}
	}

	void Texture::submitInvalidate() {
		Renderer::submit([instance = Ref<Texture>(this)] {
			instance->invalidate();
		});
	}

	void Texture::bind(uint32_t slot /*= 0*/) const {
		//ASSERT(!m_dirty, "Texture is dirty, call invalidate() before binding");
		Renderer::submit([instance = Ref<const Texture>(this), slot] {
			GL(glBindTextureUnit(slot, instance->m_handle));
		});
	}

	void Texture::unbind(uint32_t slot /*= 0*/) const {
		Renderer::submit([slot] {
			GL(glBindTextureUnit(slot, 0));
		});
	}

	void Texture::resize(uint32_t width, uint32_t height) {
		if (m_width == width && m_height == height) return;

		m_width = width;
		m_height = height;

		if (m_desc.samples != MSAA::NONE) {
			invalidate();
		} else {
			if (!m_desc.isImmutable) {
				GL(glBindTexture(GL_TEXTURE_2D, m_handle));
				GL(glTexImage2D(GL_TEXTURE_2D, 0, m_desc.getInternalFormat(), m_width, m_height, 0, m_desc.getImageFormat(), m_desc.getDataType(), nullptr));
				if (m_desc.hasMipmaps) GL(glGenerateMipmap(GL_TEXTURE_2D));
			} else {
				invalidate();
			}
		}
	}

	void Texture::saveAsPNG(const std::string& file) {
		if (m_desc.readWrite && m_buffer) {
			uint32_t channels = m_desc.getBytesPerPixel();
			stbi_write_png(file.c_str(), getWidth(), getHeight(), channels, m_buffer.data(), channels * getWidth());
			Log::info("[Texture] saved texture to {}", file.c_str());
		} else Log::warn("[Texture] failed to save texture because texture has no data");
	}
}