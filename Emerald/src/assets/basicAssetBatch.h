#pragma once

class BasicAssetBatch : public AssetBatch {
private:
	bool m_started = false;
	bool m_finished = false;
	int m_currentBatchSize = 0;
	int m_currentBatchProgress = 0;

	int m_currentQueueSize = 0;
	int m_currentQueueProgress = 0;

	queue<AsyncQueue<AssetLoader*>*> m_queueQueue;
	AsyncQueue<AssetLoader*>* m_aSyncQueue;
	AsyncQueue<AssetLoader*> m_syncQueue;

	BasicAssetBatch(const String& name) : AssetBatch(name) {
		PushLayer();
	}

	friend class AssetManager;

	bool PopQueue() {
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

public:
	~BasicAssetBatch() override {
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

	void PushLayer() {
		m_queueQueue.push(new AsyncQueue<AssetLoader*>());
		m_aSyncQueue = m_queueQueue.back();
	}

	void Add(AssetLoader* loader) override {
		if (m_started) LOG_ERROR("[~yAssets~x] Asset batch ~1%s~r already started");
		m_aSyncQueue->Add(loader);
		m_currentBatchSize++;
	}

	void Start() {
		m_aSyncQueue = nullptr;
		PopQueue();
	}


	void Update(AssetManager* manager) override {
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
				LOG("[~yAssets~x] Finished asset batch ~1%s~x", m_name.c_str());
			}
		}
	}

	void AsyncUpdate() override {
		if (m_finished) return;
		AssetLoader* loader;
		if (m_aSyncQueue->WaitForGet(loader)) {
			loader->AsyncLoad();
			m_syncQueue.Add(loader);
		}
	}

	//void Load(map<String, AssetBase*>& assets) override {
	//	if (m_finished) return;
	//	m_currentBatchSize = 0;
	//	m_currentBatchProgress = 0;
	//	m_currentBatchSize = m_queue.size();
	//
	//	m_started = true;
	//	Timer timer;
	//	for (AssetLoader* loader : m_queue) {
	//		loader->AsyncLoad();
	//		loader->SyncLoad(assets);
	//		m_currentBatchProgress++;
	//		GetGLFiberManager()->GoToMainFiber();
	//	}
	//	m_finished = true;
	//	LOG("[~yAssets~x] Finished asset batch ~1%s~x in %.2fms", m_name.c_str(), timer.Get());
	//}
	bool IsFinished() override { return m_finished; }

	float GetProgress() override { return (float)m_currentBatchProgress / (float)m_currentBatchSize; }
};
