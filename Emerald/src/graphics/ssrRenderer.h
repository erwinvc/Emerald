#pragma once

class RenderingPipeline;
class SSRRenderer
{
private:
	//AssetRef<FrameBuffer> m_fbo;
	//AssetRef<Texture> m_texture;
	AssetRef<Shader> m_shader;
	AssetRef<Mesh> m_quad;
	
public:
	bool m_enabled = true;
	SSRRenderer();
	void Draw(RenderingPipeline* pipeline);
	void OnImGui();
};
