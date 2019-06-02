#include "stdafx.h"

FrameBuffer::~FrameBuffer() {
	GL(glDeleteFramebuffers(1, &m_fbo));
	LOG("Deleted %d texture", m_textures.size());
	for (Texture* tex : m_textures) DELETE(tex);
}

bool FrameBuffer::CheckStatus() {
	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		LOG_ERROR("[~cBuffers~x] ~rFramebuffer %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
		return false;
	} else return true;
}
void FrameBuffer::Initialize() {
	GL(glGenFramebuffers(1, &m_fbo));
	GL(glGenRenderbuffers(1, &m_dbo));

	GL(glBindRenderbuffer(GL_RENDERBUFFER, m_dbo));
	GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, m_width, m_height));
	GL(glBindRenderbuffer(GL_RENDERBUFFER, 0));

	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
	GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dbo));

	if (CheckStatus()) LOG("[~cBuffers~x] Framebuffer %s initialized", m_name.c_str());
}

void FrameBuffer::AddColorBuffer(Texture* texture) {
	m_textures.push_back(texture);
	Bind();

	GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + m_colorAttachments, GL_TEXTURE_2D, texture->GetHandle(), 0));

	m_colorAttachments++;
	GL(glDrawBuffers(m_colorAttachments, drawBuffers));

	GL(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));
	if (status != GL_FRAMEBUFFER_COMPLETE) LOG_ERROR("[~cBuffers~x] ~radding attachment to %s failed: %s", m_name.c_str(), GLUtils::GetFBOStatus(status));
	Unbind();
}