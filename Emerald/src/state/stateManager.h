#pragma once

class VoxelState;
class GameStates {
public:
	static State* LOADING;
	static State* EDITOR;
	static State* MENU;
	static State* GAME;
	static State* PAUSE;
	static VoxelState* VOXEL;
};

class StateManager : public Singleton<StateManager > {
private:
	StateManager() {}
	~StateManager() {
		Cleanup();
	}
	friend Singleton;

	vector<State*> m_states;
	State* m_currentState;
	State* m_nextState = nullptr;
	State* m_nextRemoveState = nullptr;

public:
	State* GetState() { return m_currentState; }

	void RegisterStates();
	void RemoveState(State* state);

	void OnResize(int width, int height);
	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type);
	void Update(TimeStep time);
	void RenderGeometry(HDRPipeline* pipeline);
	void OnStateImGUI();
	void OnImGUI();
	void Cleanup();

	template<typename T>
	T* RegisterState() {
		static_assert(is_base_of<State, T>::value, "type parameter of this class must derive from State");
		for (State* state : m_states) {
			if (typeid(state) == typeid(T)) {
				LOG_ERROR("[~mGameState~x] already registered ~1%s~x", state->GetName().c_str());
				return (T*)state;
			}
		}
		T* instance = NEW(T());

		m_states.push_back(instance);
		LOG("[~mGameState~x] registered ~1%s ~xstate", instance->GetName().c_str());
		return (T*)instance;
	}

	inline vector<State*> GetStates() { return m_states; }
	inline void SetState(State* state) { m_nextState = state; }
};

inline StateManager* GetStateManager() { return StateManager::GetInstance(); }