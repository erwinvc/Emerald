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

public:
	State* GetState() { return m_currentState; }

	void RegisterStates();
	void RemoveState(State* state);
	void SetState(State* state) {
		m_currentState->OnExitState();
		m_currentState = state;
		m_currentState->OnEnterState();
	}

	void OnResize(int width, int height) {
		for (State* state : m_states) state->OnResize(width, height);
	}

	void RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) { m_currentState->RenderGeometryShadow(pipeline, type); }
	void Update(TimeStep time) { m_currentState->Update(time); }
	void RenderGeometry(HDRPipeline* pipeline) { m_currentState->RenderGeometry(pipeline); }
	void OnStateImGUI() {
		if (ImGui::BeginTabItem("State")) {
			int i = 0;
			for (State* state : m_states) {
				if (ImGui::Selectable(state->GetName().c_str(), m_currentState == state)) {
					SetState(state);
				}
			}
			if (ImGui::CollapsingHeader("Current state")) m_currentState->OnStateImGUI();
			ImGui::EndTabItem();
		}
	}

	void OnImGUI() {
		m_currentState->OnImGUI();
	}
	void Cleanup() {
		for (State* state : m_states) {
			state->Cleanup();
			DELETE(state);
		}
		m_states.clear();
	}

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

	vector<State*> GetStates() {
		return m_states;
	}
};

static StateManager * GetStateManager() { return StateManager::GetInstance(); }