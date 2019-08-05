#include "stdafx.h"


State* GameStates::LOADING = nullptr;
State* GameStates::MENU = nullptr;
State* GameStates::LOADINGWORLD = nullptr;
State* GameStates::GAME = nullptr;
State* GameStates::MAINMENU = nullptr;

void StateManager::RegisterStates() {
	GameStates::LOADING = RegisterState<LoadingState>();
	GameStates::MENU = RegisterState<MenuState>();
	GameStates::LOADINGWORLD = RegisterState<LoadingWorldState>();
	GameStates::GAME = RegisterState<GameState>();
	GameStates::MAINMENU = RegisterState<MainMenuState>();

	GameStates::LOADING->Initialize();
	m_currentState = GameStates::LOADING;
	m_currentState->OnEnterState();
}