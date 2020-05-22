#pragma once

//class ShadowMapGenerator : public Singleton<ShadowMapGenerator> {
//private:
//	FrameBuffer m_fbo;
//	bool m_initialized = false;
//public:
//	void Initialize() {
//		if (m_initialized) return;
//
//		GL(glGenTextures(1, &m_handle));
//
//		glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
//		for (unsigned int i = 0; i < 6; ++i)
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//		m_fbo = GetFrameBufferManager()->Create("DepthCubemap", SHADOW_WIDTH, SHADOW_HEIGHT);
//
//		m_fbo->Bind();
//		GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_handle, 0));
//		GL(glDrawBuffer(GL_NONE));
//		GL(glReadBuffer(GL_NONE));
//
//		m_fbo->Unbind();
//
//		m_shader = GetShaderManager()->Get("DepthCubemap");
//
//		m_initialized = true;
//
//	}
//}