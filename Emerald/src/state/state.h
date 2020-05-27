#pragma once

class State {
	String m_name;
public:
	State(const String& name) : m_name(name) {}
	virtual ~State() {}

	String GetName() { return m_name; }
	
	virtual void Initialize() = 0;
	virtual void Update(const TimeStep& time) = 0;
	virtual void RenderGeometry(HDRPipeline* pipeline) = 0;
	virtual void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) = 0;
	//virtual void RenderUI() = 0;
	virtual void OnStateImGUI() = 0;
	virtual void OnImGUI() = 0;
	virtual void Cleanup() = 0;

	virtual void OnEnterState() = 0;
	virtual void OnExitState() = 0;
	virtual void OnResize(int width, int height) = 0;
};