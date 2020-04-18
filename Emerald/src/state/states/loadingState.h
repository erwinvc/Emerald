#pragma once

class LoadingState : public State {
private:
	String m_name = "Loading";
	AssetRef<Texture> m_logo;
	AssetRef<Texture> m_loadingTexture;
	AssetRef<BasicAssetBatch> m_batch;
public:
	const String& GetName() override { return m_name; }

	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(Shader* overrideShader = nullptr) override;
	void RenderUI() override;
	void OnStateImGUI() override;
	void OnImGUI() override;
	void Cleanup() override;

	void OnEnterState() override;
	void OnExitState() override;
	void OnResize(int width, int height) override;
};