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

void StateManager::RemoveState(State* state) {
	if (m_nextRemoveState != nullptr) LOG_ERROR("[~mGameState~x] next remove state has already been set");
	m_nextRemoveState = state;
}