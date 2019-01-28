#include "stdafx.h"

void FrameBuffer::Initialize() {
    GL(glGenFramebuffers(1, &m_fbo));
    GL(glGenRenderbuffers(1, &m_dbo));

    m_texture = new Texture(m_width, m_height, TextureParameters(RGBA16, LINEAR, REPEAT, T_FLOAT));

    GL(glBindRenderbuffer(GL_RENDERBUFFER, m_dbo));
    GL(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_width, m_height));

    GL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture->GetHandle(), 0));
    GL(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_dbo));
    GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}