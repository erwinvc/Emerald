#pragma once

class DepthCubemap {
private:
	uint m_handle;
	FrameBuffer* m_fbo;
	const unsigned int SHADOW_WIDTH = 128, SHADOW_HEIGHT = 128;
	std::vector<glm::mat4> shadowTransforms;
	glm::mat4 shadowProj;
	float m_nearPlane = 0.0f;
	float m_farPlane = 15.0f;
	Shader* m_shader;

public:

	DepthCubemap();
	~DepthCubemap();

	void Draw(HDRPipeline* pipeline, glm::vec3 lightpos);
	void OnImGui();

	inline uint GetHandle() { return m_handle; }
};