#include "stdafx.h"

void CustomLoader::AsyncLoad() {
	m_async();
}

void CustomLoader::SyncLoad(AssetManager* manager) {
	m_sync();
}

float CustomLoader::GetProgress() {
	return 0;
}