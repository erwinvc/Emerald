#include "stdafx.h"

FrameBuffer::FrameBuffer(String name, uint width, uint height, Color& clearColor) : m_name(name), m_width(width), m_height(height), m_color(clearColor) {
	GL(glGenFramebuffers(1, &m_fbo));
	GL(glGenRenderbuffers(1, &m_dbo));

	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_dbo));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_width, m_height));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dbo));

	if (CheckStatus()) LOG("[~cBuffers~x] Created ~1%s~x framebuffer", m_name.c_str());
}

FrameBuffer::~FrameBuffer() {
	GL(glDeleteFramebuffers(1, &m_fbo));
	LOG("[~cBuffers~x] Deleted %d texture", m_textures.size());
}

bool FrameBuffer::CheckStatus() {
	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("[~cBuffers~x] ~rFramebuffer %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
		return false;
	} else return true;
}

void FrameBuffer::AddColorBuffer(AssetRef<Texture> texture) {
	m_textures.push_back(texture);
	Bind();

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorAttachments, GL_TEXTURE_2D, texture->GetHandle(), 0));

	m_colorAttachments++;
	GL(glDrawBuffers(m_colorAttachments, drawBuffers));

	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[~cBuffers~x] ~radding attachment to %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
	Unbind();
}