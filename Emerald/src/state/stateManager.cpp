#include "stdafx.h"

State* GameStates::LOADING = nullptr;
State* GameStates::EDITOR = nullptr;
State* GameStates::MENU = nullptr;
State* GameStates::GAME = nullptr;
State* GameStates::PAUSE = nullptr;
State* GameStates::GAMEOVER = nullptr;

void StateManager::RegisterStates() {
	GameStates::LOADING = RegisterState<LoadingState>();
	GameStates::EDITOR = RegisterState<EditorState>();
	GameStates::MENU = RegisterState<MenuState>();
	GameStates::GAME = RegisterState<GameState>();
	GameStates::PAUSE = RegisterState<PauseState>();
	GameStates::GAMEOVER = RegisterState<GameOverState>();

	GameStates::LOADING->Initialize();
	m_currentState = GameStates::LOADING;
	m_currentState->OnEnterState();
}

void StateManager::RemoveState(State* state) {
	Utils::RemoveFromVector(m_states, state);
	state->Cleanup();
	DELETE(state);
}
