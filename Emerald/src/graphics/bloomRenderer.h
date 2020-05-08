#pragma once

class HDRPipeline;
class Mesh;
class BloomRenderer {
private:
	AssetRef<FrameBuffer> m_bloomFBO;
	AssetRef<Texture> m_bloomTexture;
	AssetRef<FrameBuffer> m_pingPongFBO[2];
	AssetRef<Texture> m_pingPongTexture[2];
	AssetRef<Shader> m_gaussianShader;
	AssetRef<Shader> m_bloomShader;
	AssetRef<Mesh> m_quad;

public:
	float m_gaussianIterations = 8;
	bool m_enabled = true;
	float m_bloomFactor = 1.0f;
	float m_bloomMultiplier = 1.0f;
	
	BloomRenderer();

	void Draw(HDRPipeline* pipeline);

	Texture* GetBloomedTexture() {
		return m_pingPongTexture[1];
	}

	void OnImGui() {
		//UI::Float("Factor", &m_bloomFactor, 0, 2.0f);
		UI::Float("Multiplier", &m_bloomMultiplier, 0, 5.0f);
		//UI::Float("Chromatic", &m_chromatic, -0.01f, 0.01f);
	}
};