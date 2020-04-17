#pragma once

class RenderingPipeline;
class ShadowRenderer
{
private:
	FrameBuffer* m_fbo;
	uint m_width;
	uint m_height;
	
public:
	ShadowRenderer(uint width, uint height);
	void Begin(RenderingPipeline* pipeline);
	void End(RenderingPipeline* pipeline);
	void OnImGui();
};