#pragma once

class MenuState : public State {
private:
	char nameBuffer[16];

public:
	MenuState() : State("Menu") {}
	ServerStatus m_status;
	float dotDotDotTimer = 0;

	String GetDotDotDot(const String& text);
	void Buttons();
	
	void Initialize() override;
	void Update(const TimeStep& time) override;
	void RenderGeometry(HDRPipeline* pipeline) override {}
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override {}
	void OnStateImGUI() override {}
	void OnImGUI() override;
	void Cleanup() override {}
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};