#pragma once
#include <util/color.h>
#include "textureDesc.h"

namespace emerald {
	class Texture;

	enum FBOScale {
		STATIC,
		FULL,
		HALF,
		QUARTER,
		ONEEIGHTH
	};

	enum class FBOAttachment {
		COLOR,
		DEPTH,
		STENCIL,
		DEPTHSTENCIL
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
		uint32_t samples = 1;
	};

	class FrameBuffer {
	private:
		std::shared_ptr<Texture> m_depthTexture;
		std::vector<std::shared_ptr<Texture>> m_textures;
		std::vector<std::string> m_textureNames;
		uint32_t m_colorAttachments = 0;
		uint32_t m_handle = 0;

		FrameBuffer(FramebufferDesc desc);


		bool checkStatus();

		std::shared_ptr<Texture> addBuffer(const std::string& name, TextureFormat format, FBOAttachment type);

		float fboScaleToFloat(FBOScale scale) const;

		FramebufferDesc m_desc;

	public:
		~FrameBuffer();

		const FramebufferDesc& descriptor() const { return m_desc; }

		static std::shared_ptr<FrameBuffer> create(FramebufferDesc desc);

		void bind() const;
		void unbind() const;
		void clear() const;
		void clearDepthOnly() const;
		void clearColorOnly() const;
		void clearStencilOnly() const;
		void setDrawAndReadBuffersToNone() const;
		void setScale(FBOScale scale);

		void blit(FrameBuffer* targetFBO) const;
		void blitColorOnly(FrameBuffer* targetFBO) const;
		void blitDepthOnly(FrameBuffer* targetFBO) const;

		void resize(uint32_t width, uint32_t height, bool forceRecreate);

		uint32_t getWidth() const { return m_desc.width; }
		uint32_t getHeight() const { return m_desc.height; }
		uint32_t getScaledWidth() const { return (uint32_t)(fboScaleToFloat(m_desc.scale) * m_desc.width); }
		uint32_t getScaledHeight() const { return (uint32_t)(fboScaleToFloat(m_desc.scale) * m_desc.height); }

		uint32_t handle() const { return m_handle; }

		std::vector<std::shared_ptr<Texture>>& getTextures() { return m_textures; }
		std::vector<std::string>& getTextureNames() { return m_textureNames; }
		std::shared_ptr<Texture> getDepthTexture() { return m_textures[0]; }
	};

	namespace FrameBufferManager {
		inline std::vector<std::weak_ptr<FrameBuffer>> m_frameBuffers;
		void add(const std::shared_ptr<FrameBuffer>& fbo);
		void bindDefaultFBO();
		void onResize(uint32_t width, uint32_t height);
		void shutdown();
	};
}