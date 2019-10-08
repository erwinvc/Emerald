#include "stdafx.h"

StateLoader::StateLoader(State* state) : AssetLoader(state->GetName()), m_state(state) {}

void StateLoader::SyncLoad(AssetManager* manager) {
	Timer timer;
	m_state->Initialize();
	LOG("[~mGameState~x] initialized ~1%s~x in %.2fms", m_state->GetName().c_str(), timer.Get());
}