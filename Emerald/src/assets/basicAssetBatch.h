#pragma once

class BasicAssetBatch : public AssetBatch {
private:
	bool m_started = false;
	bool m_finished = false;
	int m_currentBatchSize = 0;
	int m_currentBatchProgress = 0;
	vector<AssetLoader*> m_queue;

	BasicAssetBatch(const String& name) : AssetBatch(name) {}

	//friend AssetManager;
public:
	~BasicAssetBatch() override {
		for (AssetLoader* loader : m_queue)
			DELETE(loader);
	}

	void Add(AssetLoader* loader) override {
		if (m_started) LOG_ERROR("[~yAssets~x] Asset batch ~1%s~r already started");
		m_queue.push_back(loader);
	}

	void Load(map<String, AssetBase*>& assets) override {
		if (m_finished) return;
		m_currentBatchSize = 0;
		m_currentBatchProgress = 0;
		m_currentBatchSize = m_queue.size();

		m_started = true;
		Timer timer;
		for (AssetLoader* loader : m_queue) {
			loader->AsyncLoad();
			loader->SyncLoad(assets);
			m_currentBatchProgress++;
			GetGLFiberManager()->GoToMainFiber();
		}
		m_finished = true;
		LOG("[~yAssets~x] Finished asset batch ~1%s~x in %.2fms", m_name.c_str(), timer.Get());
	}
	bool IsFinished() override { return m_finished; }

	float GetProgress() override { return 1; }
};
