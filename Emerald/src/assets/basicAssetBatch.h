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

	bool PopQueue();

	friend class AssetManager;

public:
	~BasicAssetBatch() override;

	void PushLayer();

	void Add(AssetLoader* loader) override;
	void Start();
	void Update(AssetManager* manager) override;
	void AsyncUpdate() override;
	
	inline bool IsFinished() override { return m_finished; }
	inline float GetProgress() override { return (float)m_currentBatchProgress / (float)m_currentBatchSize; }
};
