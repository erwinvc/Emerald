#pragma once

class EditorState : public State {
private:
	String m_name = "Editor";
	void OnImGuiViewport();
	void CameraControls();

	ImGuiID m_dockspaceCenter;
	ImGuiID m_dockspaceLeft;
	ImGuiID m_dockspaceRight;
	ImGuiID m_dockspaceBottom;
	World* m_world;

public:
	const String& GetName() override { return m_name; }

	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(Shader* overrideShader = nullptr) override;
	void RenderUI() override {}
	void OnStateImGUI() override {}
	void OnImGUI() override;
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};