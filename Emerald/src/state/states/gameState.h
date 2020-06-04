#pragma once

class GameState : public State {
private:
public:
	GameState() : State("Game") {}

	void Initialize() override {}

	void Update(const TimeStep& time) override {
		Camera::active->Update();
	}
	void RenderGeometry(HDRPipeline* pipeline) override {}
	void OnStateImGUI() override {}
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) override {}
	void OnImGUI() override {}
	void Cleanup() override {}
	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};