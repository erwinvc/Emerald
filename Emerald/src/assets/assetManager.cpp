#include "stdafx.h"

AssetManager::~AssetManager() {
	for (auto it = m_assets.begin(); it != m_assets.end(); it++) {
		for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
			DELETE(it2->second);
		}
	}
}

void AssetManager::SubmitBatch(AssetBatch* batch) {
	LOG("[~yResource~x] asset batch ~1%s~x submitted", batch->GetName().c_str());
	m_currentBatch = batch;
	m_currentBatch->Start();
}

void AssetManager::Update() {
	if (m_currentBatch) {
		m_currentBatch->Update(this);
		m_currentBatch->AsyncUpdate();
	}
}

float AssetManager::GetProgress() {
	if (!m_currentBatch) return 0;
	return m_currentBatch->GetProgress();
}