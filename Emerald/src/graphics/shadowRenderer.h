#pragma once

class HDRPipeline;
class ShadowRenderer
{
private:
	FrameBuffer* m_fbo;
	uint m_width;
	uint m_height;
	
public:
	ShadowRenderer(uint width, uint height);
	void Begin(HDRPipeline* pipeline);
	void End(HDRPipeline* pipeline);
	void OnImGui();
};