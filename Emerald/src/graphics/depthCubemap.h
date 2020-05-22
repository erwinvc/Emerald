#pragma once

class DepthCubemap {
private:
	FrameBuffer* m_fbo;
	const unsigned int SHADOW_WIDTH = 256, SHADOW_HEIGHT = 256;
	std::vector<glm::mat4> shadowTransforms;
	glm::mat4 shadowProj;
	float m_nearPlane = 0.0f;
public:
	float m_farPlane = 15.0f;
	Shader* m_shader;
	uint m_handle;
	DepthCubemap() {
		GL(glGenTextures(1, &m_handle));

		glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
		for (unsigned int i = 0; i < 6; ++i)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		m_fbo = GetFrameBufferManager()->Create("DepthCubemap", SHADOW_WIDTH, SHADOW_HEIGHT);

		m_fbo->Bind();
		GL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_handle, 0));
		GL(glDrawBuffer(GL_NONE));
		GL(glReadBuffer(GL_NONE));

		m_fbo->Unbind();

		m_shader = GetShaderManager()->Get("DepthCubemap");
	}

	void Draw(HDRPipeline* pipeline, glm::vec3 lightpos) {
		float aspect = (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT;
		shadowProj = glm::perspective(glm::radians(90.0f), aspect, m_nearPlane, m_farPlane);

		shadowTransforms.clear();
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadowTransforms.push_back(shadowProj * glm::lookAt(lightpos, lightpos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		m_fbo->Bind();
		m_fbo->ClearDepthOnly();
		m_shader->Bind();
		m_shader->Set("shadowMatrices", shadowTransforms.data(), shadowTransforms.size());
		m_shader->Set("far_plane", m_farPlane);
		m_shader->Set("lightPos", lightpos);
	}

	void OnImGui() {
		UI::Begin();
		UI::Float("Far", &m_farPlane, 1.1f, 50.0f);
		UI::Float("Neat", &m_nearPlane, 0.0f, 2.0f);
		UI::End();
	}

	//void BindCubemap() {
	//	glActiveTexture(GL_TEXTURE5);
	//	glBindTexture(GL_TEXTURE_CUBE_MAP, m_handle);
	//}
};