#include "stdafx.h"


State* GameStates::LOADING = nullptr;
State* GameStates::MENU = nullptr;
State* GameStates::LOADINGWORLD = nullptr;
State* GameStates::GAME = nullptr;
State* GameStates::MAINMENU = nullptr;
State* GameStates::LUDO = nullptr;

void StateManager::RegisterStates() {
	GameStates::LOADING = RegisterState<LoadingState>();
	GameStates::MENU = RegisterState<MenuState>();
	GameStates::LOADINGWORLD = RegisterState<LoadingWorldState>();
	GameStates::GAME = RegisterState<GameState>();
	GameStates::MAINMENU = RegisterState<MainMenuState>();
	GameStates::LUDO = RegisterState<LudoState>();

	GameStates::LOADING->Initialize();
	m_currentState = GameStates::LOADING;
	m_currentState->OnEnterState();
}

void StateManager::RemoveState(State* state) {
	Utils::RemoveFromVector(m_states, state);
	state->Cleanup();
	DELETE(state);
}
