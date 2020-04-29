#pragma once

class HDRPipeline;
class Mesh;
class SSRRenderer
{
private:
	AssetRef<FrameBuffer> m_fbo;
	AssetRef<Texture> m_texture;
	AssetRef<Shader> m_shader;
	AssetRef<Shader> m_simpleShader;
	AssetRef<Mesh> m_quad;
	
public:
	bool m_enabled = true;
	SSRRenderer();
	void Draw(HDRPipeline* pipeline);
	void OnImGui();
};
