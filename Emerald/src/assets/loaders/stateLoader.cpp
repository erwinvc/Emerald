#include "stdafx.h"

StateLoader::StateLoader(State* state) : AssetLoader(state->GetName(), true), m_state(state) {}

void StateLoader::SyncLoad(map<String, AssetBase*>& assets) {
	Timer timer;
	m_state->Initialize();
	LOG("[~mGameState~x] initialized ~1%s~x in %.2fms", m_state->GetName().c_str(), timer.Get());
}