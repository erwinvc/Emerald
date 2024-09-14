#pragma once
#include <util/color.h>
#include "textureDesc.h"
#include "texture.h"

namespace emerald {

	enum FBOScale {
		STATIC,
		FULL,
		HALF,
		QUARTER,
		ONEEIGHTH
	};

	struct FramebufferAttachmentDesc {
		std::string name;
		TextureFormat format;
	};

	struct FramebufferDesc {
		std::string name = "";
		FBOScale scale = FBOScale::FULL;
		uint32_t width = 0;
		uint32_t height = 0;
		Color clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		std::vector<FramebufferAttachmentDesc> attachments;
		MSAA samples = MSAA::NONE;
	};

	class FrameBuffer : public RefCounted {
	private:
		Ref<Texture> m_depthTexture;
		std::vector<Ref<Texture>> m_textures;
		std::vector<std::string> m_textureNames;
		uint32_t m_colorAttachments = 0;
		uint32_t m_handle = 0;

		FrameBuffer(FramebufferDesc desc);

		bool checkStatus();
		void invalidateTextures();
		void attachTextures();
		float fboScaleToFloat(FBOScale scale) const;

		FramebufferDesc m_desc;

	public:
		~FrameBuffer();

		const FramebufferDesc& descriptor() const { return m_desc; }

		static const Ref<FrameBuffer>& create(FramebufferDesc desc);

		void bind() const;
		void unbind() const;
		void clear() const;
		void clearDepthOnly() const;
		void clearColorOnly() const;
		void clearStencilOnly() const;
		void setDrawAndReadBuffersToNone() const;
		void setScale(FBOScale scale);
		void setMSAA(MSAA msaa);
		void blit(const Ref<FrameBuffer>& targetFBO) const;
		void blitColorOnly(const Ref<FrameBuffer>& targetFBO) const;
		void blitDepthOnly(const Ref<FrameBuffer>& targetFBO) const;

		void resize(uint32_t width, uint32_t height, bool forceRecreate);

		uint32_t getWidth() const { return m_desc.width; }
		uint32_t getHeight() const { return m_desc.height; }
		uint32_t getScaledWidth() const { return (uint32_t)(fboScaleToFloat(m_desc.scale) * m_desc.width); }
		uint32_t getScaledHeight() const { return (uint32_t)(fboScaleToFloat(m_desc.scale) * m_desc.height); }

		uint32_t handle() const { return m_handle; }

		std::vector<Ref<Texture>>& getTextures() { return m_textures; }
		std::vector<std::string>& getTextureNames() { return m_textureNames; }
		const Ref<Texture>& getDepthTexture() { return m_textures[0]; }
	};

	namespace FrameBufferManager {
		inline std::vector<Ref<FrameBuffer>> m_frameBuffers;
		void add(const Ref<FrameBuffer>& fbo);
		void bindDefaultFBO();
		void onResize(uint32_t width, uint32_t height);
		void shutdown();
	};
}