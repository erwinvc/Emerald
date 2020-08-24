#pragma once

class PauseState : public State {
private:
public:
	PauseState() : State("Pause") {}


	void Initialize() override {}
	void Update(const TimeStep& time) override {
	}
	void RenderGeometry(HDRPipeline* pipeline) override {}
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override {}
	void OnStateImGUI() override {}
	void OnImGUI() override {}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};