#pragma once

enum class AmbientOcclusionType {
	SSAO,
	HBAO
};

class Mesh;
class AmbientOcclusionRenderer {
private:
	AssetRef<FrameBuffer> m_fbo;
	AssetRef<FrameBuffer> m_fboBlur;
	AssetRef<Texture> m_texture;
	AssetRef<Texture> m_textureBlur;
	AssetRef<Texture> m_noiseTexture;
	AssetRef<Shader> m_ssaoShader;
	AssetRef<Shader> m_ssaoBlurShader;
	AssetRef<Shader> m_hbaoShader;
	AssetRef<Material> m_ssaoMaterial;
	AssetRef<Material> m_ssaoBlurMaterial;
	AssetRef<Material> m_hbaoMaterial;
	AssetRef<Mesh> m_quad;

	vector<glm::vec3> m_ssaoKernels;

	AmbientOcclusionType m_aoType = AmbientOcclusionType::HBAO;

	void DrawSSAO(HDRPipeline* pipeline);
	void DrawHBAO(HDRPipeline* pipeline);

public:
	
	bool m_enabled = true;

	const int KERNELCOUNT = 64;

	float m_ssaoRadius = 1.0f;
	float m_ssaoBias = 0.09f;
	int m_ssaoPower = 2;
	int m_ssaoSampleCount = 64;

	int m_hbaoPower = 2;
	float m_hbaoLumInfluence = 0.2f;
	float m_hbaoAngleBias = 0.2f;
	float m_hbaoAttenuationScale = 1.0f;
	int m_hbaoSampleDirections = 8.0f;   
	int m_hbaoSampleSteps = 64.0f;
	float m_hbaoSampleRadius = 9.0f;
	
	AmbientOcclusionRenderer();
	~AmbientOcclusionRenderer();

	void Draw(HDRPipeline* pipeline);

	Texture* GetTexture();
	Texture* GetRawTexture() { return m_texture; }

	void OnImGui();
};