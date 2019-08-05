#pragma once

class GameStates {
public:
	static State* LOADING;
	static State* MENU;
	static State* LOADINGWORLD;
	static State* GAME;
	static State* MAINMENU;
};

class StateManager : public Singleton<StateManager > {
private:
	StateManager() {}
	~StateManager() {
		for (State* state : m_states) {
			DELETE(state);
		}
	}
	friend Singleton;

	vector<State*> m_states;
	State* m_currentState;

public:
	State* GetState() { return m_currentState; }

	void RegisterStates();

	void SetState(State* state) {
		m_currentState->OnExitState();
		m_currentState = state;
		m_currentState->OnEnterState();
	}

	void OnResize(int width, int height) {
		for (State* state : m_states) state->OnResize(width, height);
	}

	void Update(TimeStep time) { m_currentState->Update(time); }
	void RenderGeometry() { m_currentState->RenderGeometry(); }
	void RenderUI() { m_currentState->RenderUI(); }
	void OnImGUI() { m_currentState->OnImGUI(); }
	void Cleanup() { m_currentState->Cleanup(); }

	template<typename T>
	State* RegisterState() {
		static_assert(is_base_of<State, T>::value, "type parameter of this class must derive from State");
		for (State* state : m_states) {
			if (typeid(state) == typeid(T)) {
				LOG_ERROR("[~mGameState~x] ~1%s~x already registered", state->GetName().c_str());
				return (T*)state;
			}
		}
		T* instance = NEW(T());

		m_states.push_back(instance);
		LOG("[~mGameState~x] registered ~1%s ~xstate", instance->GetName().c_str());
		return (T*)instance;
	}

	vector<State*> GetStates() {
		return m_states;
	}
};

static StateManager * GetStateManager() { return StateManager::GetInstance(); }