#pragma once

class AssetManager;

class AssetBatch {
private:
	String m_name;
	bool m_started = false;
	bool m_finished = false;
	vector<AssetLoader*> m_queue;

	AssetBatch(const String& name) : m_name(name) {}

	friend AssetManager;
public:
	~AssetBatch() {
		for (AssetLoader* loader : m_queue)
			DELETE(loader);
	}


	template<class T>
	void Add(T* loader) {
		if (m_started) LOG_ERROR("[~yAssets~x] Asset batch ~1%s~r already started");
		m_queue.push_back(loader);
	}

	inline bool IsFinished() { return m_finished; }
};

class AssetManager : public Singleton<AssetManager> {
	Timer m_timer;

	map<String, AssetBase*> m_assets;
	ManagedRef<AssetBatch> m_currentBatch;

	const int THREADCOUNT = 4;
	int m_currentBatchSize = 0;
	int m_currentBatchProgress = 0;

	AssetManager() {
		for (int i = 0; i < THREADCOUNT; i++)
			GetThreadManager()->RegisterThread(Format("AssetManagerPool%d", i), [] {});
	}
	~AssetManager() {
		for (auto it = m_assets.begin(); it != m_assets.end(); it++) {
			DELETE(it->second);
		}
	}
	friend Singleton;

public:
	ManagedRef<AssetBatch> CreateBatch(const String& name) {
		LOG("[~yAssets~x] Asset batch ~1%s~x created", name.c_str());
		return NEW(AssetBatch(name));
	}

	void SubmitBatch(ManagedRef<AssetBatch> batch) {
		LOG("[~yAssets~x] Asset batch ~1%s~x submitted", batch->m_name.c_str());
		m_currentBatch = batch;
		m_currentBatchSize = 0;
		m_currentBatchProgress = 0;
		//for (auto pair = m_currentBatch->m_queue.begin(); pair != m_currentBatch->m_queue.end(); ++pair) {
		//	m_currentBatchSize += pair->second.size();
		//}
		m_currentBatchSize = m_currentBatch->m_queue.size();
	}

	void Update() {
		if (m_currentBatch) {
			m_currentBatch->m_started = true;
			m_timer = Timer();
			for (AssetLoader* loader : m_currentBatch->m_queue) {
				loader->AsyncLoad();
				loader->SyncLoad(m_assets);
				m_currentBatchProgress++;
				GetGLFiberManager()->GoToMainFiber();
			}
			m_currentBatch->m_finished = true;
			LOG("[~yAssets~x] Finished asset batch ~1%s~x in %.2fms", m_currentBatch->m_name.c_str(), m_timer.Get());
			m_currentBatch = nullptr;
		}
	}

	float GetProgress() {
		return (float)m_currentBatchProgress / (float)m_currentBatchSize;
	}

	template<typename T>
	AssetRef<T> Get(const String& name) {
		return (T*)m_assets[name];
	}

	template<typename T>
	AssetRef<T> ForceLoad(AssetRef<AssetLoader> loader) {
		loader->AsyncLoad();
		loader->SyncLoad(m_assets);
		return Get<T>(loader->GetName());
	}
};

static AssetManager* GetAssetManager() { return AssetManager::GetInstance(); }