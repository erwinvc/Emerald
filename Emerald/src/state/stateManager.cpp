#include "stdafx.h"

State* GameStates::LOADING = nullptr;
State* GameStates::EDITOR = nullptr;
State* GameStates::MENU = nullptr;
State* GameStates::GAME = nullptr;
State* GameStates::PAUSE = nullptr;
VoxelState* GameStates::VOXEL = nullptr;

void StateManager::RegisterStates() {
	GameStates::LOADING = RegisterState<LoadingState>();
	GameStates::EDITOR = RegisterState<EditorState>();
	GameStates::MENU = RegisterState<MenuState>();
	GameStates::GAME = RegisterState<GameState>();
	GameStates::PAUSE = RegisterState<PauseState>();
	GameStates::VOXEL = RegisterState<VoxelState>();

	GameStates::LOADING->Initialize();
	m_currentState = GameStates::LOADING;
	m_currentState->OnEnterState();
}

void StateManager::OnResize(int width, int height) {
	for (State* state : m_states) state->OnResize(width, height);
}


void StateManager::RemoveState(State* state) {
	if (m_nextRemoveState != nullptr) LOG_ERROR("[~mGameState~x] next remove state has already been set");
	m_nextRemoveState = state;
}

void StateManager::RenderGeometryShadow(HDRPipeline* pipeline, ShadowType type) {
	m_currentState->RenderGeometryShadow(pipeline, type);
}

void StateManager::Update(TimeStep time) {
	if (m_nextState != nullptr) {
		m_currentState->OnExitState();
		m_currentState = m_nextState;
		m_currentState->OnEnterState();
		m_nextState = nullptr;
		Camera::active->Update();
	}
	if (m_nextRemoveState != nullptr) {
		Utils::RemoveFromVector(m_states, m_nextRemoveState);
		m_nextRemoveState->Cleanup();
		DELETE(m_nextRemoveState);
		m_nextRemoveState = nullptr;
	}
	m_currentState->Update(time);
}
void StateManager::RenderGeometry(HDRPipeline* pipeline) {
	m_currentState->RenderGeometry(pipeline);
}

void StateManager::OnStateImGUI() {
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

void StateManager::OnImGUI() {
	m_currentState->OnImGUI();
}

void StateManager::Cleanup() {
	for (State* state : m_states) {
		state->Cleanup();
		DELETE(state);
	}
	m_states.clear();
}