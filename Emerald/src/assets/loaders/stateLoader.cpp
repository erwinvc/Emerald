#include "stdafx.h"

StateLoader::StateLoader(State* state) : AssetLoader(state->GetName(), true), m_state(state) {}

void StateLoader::SyncLoad(map<String, AssetBase*>& assets) {
	Timer timer;
	m_state->Initialize();
	LOG("[~mGameState~x] ~1%s~x initialized in %.2fms", m_state->GetName().c_str(), timer.Get());
}