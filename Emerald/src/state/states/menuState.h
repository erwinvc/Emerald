#pragma once

class MenuState : public State {
private:
	String m_name = "Menu";
public:
	const String& GetName() override { return m_name; }

	void Initialize() override {}
	void Update(const TimeStep& time) override {}
	void RenderGeometry() override {}
	void RenderUI() override {}
	void OnStateImGUI() override {}
	void OnImGUI() override {}
	void Cleanup() override {}

	void OnEnterState() override {}
	void OnExitState() override {}
	void OnResize(int width, int height) override {}
};