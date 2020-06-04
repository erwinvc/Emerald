#include "stdafx.h"

BasicAssetBatch::~BasicAssetBatch() {
	for (int i = 0; i < m_queueQueue.size(); i++) {
		AsyncQueue<AssetLoader*>* queue = move(m_queueQueue.front());
		m_queueQueue.pop();
		for (int j = 0; j < queue->Size(); j++) {
			AssetLoader* loader;
			if (queue->TryToGet(loader)) {
				DELETE(loader);
			} else LOG_ERROR("Failed to clean!");
		}
		delete queue;
	}
}

bool BasicAssetBatch::PopQueue() {
	if (m_aSyncQueue) {
		m_aSyncQueue->ReleaseWaitingThreads();
		delete m_aSyncQueue;
	}
	if (m_queueQueue.size() > 0) {
		m_aSyncQueue = move(m_queueQueue.front());
		m_queueQueue.pop();
		m_currentQueueSize = m_aSyncQueue->Size();
		m_currentQueueProgress = 0;
		return true;
	}
	return false;
}

void BasicAssetBatch::PushLayer() {
	m_queueQueue.push(new AsyncQueue<AssetLoader*>());
	m_aSyncQueue = m_queueQueue.back();
}

void BasicAssetBatch::Add(AssetLoader* loader) {
	if (m_started) LOG_ERROR("[~yResource~x] asset batch ~1%s~r already started");
	m_aSyncQueue->Add(loader);
	m_currentBatchSize++;
}

void BasicAssetBatch::Start() {
	m_aSyncQueue = nullptr;
	PopQueue();
}


void BasicAssetBatch::Update(AssetManager* manager) {
	if (m_finished) return;
	AssetLoader* loader;
	if (m_syncQueue.TryToGet(loader)) {
		loader->SyncLoad(manager);
		DELETE(loader);
		m_currentQueueProgress++;
		m_currentBatchProgress++;
	}

	if (m_currentQueueSize == m_currentQueueProgress) {
		if (!PopQueue()) {
			m_finished = true;
			LOG("[~yResource~x] finished asset batch ~1%s~x", m_name.c_str());
		}
	}
}

void BasicAssetBatch::AsyncUpdate() {
	if (m_finished) return;
	AssetLoader* loader;
	if (m_aSyncQueue->WaitForGet(loader)) {
		loader->AsyncLoad();
		m_syncQueue.Add(loader);
	}
}