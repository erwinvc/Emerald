#pragma once

class LoadingState : public State {
private:
	AssetRef<Texture> m_logo;
	AssetRef<Texture> m_animatedLogo;
	AssetRef<Texture> m_loadingTexture;
	AssetRef<BasicAssetBatch> m_batch;
public:
	LoadingState() : State("Loading") {}
	
	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(HDRPipeline* pipeline) override;
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override;
	void OnStateImGUI() override;
	void OnImGUI() override;
	void Cleanup() override;

	void OnEnterState() override;
	void OnExitState() override;
	void OnResize(int width, int height) override;
};