#pragma once

class EditorState : public State {
private:
	void OnImGuiViewport();

	ImGuiID m_dockspaceCenter;
	ImGuiID m_dockspaceLeft;
	ImGuiID m_dockspaceRight;
	ImGuiID m_dockspaceBottom;
	ClientWorld* m_world;

public:
	EditorState() : State("Editor") {}

	void ReloadTexture(const String& file);
	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(HDRPipeline* pipeline) override;
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override;
	void OnStateImGUI() override;
	void OnImGUI() override;
	void Cleanup() override;

	void OnEnterState() override;
	void OnExitState() override;
	void OnResize(int width, int height) override {}
};