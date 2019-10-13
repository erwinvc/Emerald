#pragma once

class State {
public:
	State() {}
	virtual ~State() {}

	virtual const String& GetName() = 0;

	virtual void Initialize() = 0;
	virtual void Update(const TimeStep& time) = 0;
	virtual void RenderGeometry() = 0;
	virtual void RenderUI() = 0;
	virtual void OnStateImGUI() = 0;
	virtual void OnImGUI() = 0;
	virtual void Cleanup() = 0;

	virtual void OnEnterState() = 0;
	virtual void OnExitState() = 0;
	virtual void OnResize(int width, int height) = 0;
};