#pragma once

class VoxelState : public State {
private:
	ClientWorld* m_world;
	Texture* crosshair;
	Shader* m_directionalShadowShader;
	Shader* m_omniDirectionalShadowShader;
	bool aaa = false;
	ImGuiID m_dockspaceCenter;

public:
	Pointlight m_pointlight;
	DepthCubemap* m_dcm;
	VoxelState() : State("Voxel") {}

	~VoxelState() override;
	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(HDRPipeline* pipeline) override;
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type);
	void OnStateImGUI() override;
	void OnImGUI() override;
	void Cleanup() override {}
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}

	ClientWorld* GetWorld() { return m_world; }
};

inline ClientWorld* GetWorld() { return GameStates::VOXEL->GetWorld(); }