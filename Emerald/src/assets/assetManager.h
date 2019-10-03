#pragma once

class AssetManager : public Singleton<AssetManager> {
	Timer m_timer;

	map<String, AssetBase*> m_assets;
	AssetRef<AssetBatch> m_currentBatch;
	const int THREADCOUNT = 4;

	AssetManager() {
		//for (int i = 0; i < THREADCOUNT; i++)
		//	GetThreadManager()->RegisterThread(Format("AssetManagerPool%d", i), [] {});
	}
	~AssetManager() {
		for (auto it = m_assets.begin(); it != m_assets.end(); it++) {
			DELETE(it->second);
		}
	}
	friend Singleton;

public:

	template<typename T>
	AssetRef<AssetBatch> CreateBatch(const String& name) {
		//TODO register batches
		LOG("[~yAssets~x] Asset batch ~1%s~x created", name.c_str());
		return NEW(T(name));
	}

	void SubmitBatch(AssetRef<AssetBatch> batch) {
		LOG("[~yAssets~x] Asset batch ~1%s~x submitted", batch->GetName().c_str());
		m_currentBatch = batch;
	}

	void Update() {
		if (m_currentBatch) {
			m_currentBatch->Load(m_assets);
		}
	}

	float GetProgress() {
		if (!m_currentBatch) return 0;
		return m_currentBatch->GetProgress();
	}

	template<typename T>
	AssetRef<T> Get(const String& name) {
		T* asset = (T*)m_assets[name];
		//if (asset == nullptr) LOG_WARN("[~yAssets~x] asset ~1%s~x of type ~1%s~x not found", name.c_str(), typeid(T).name());
		return asset;
	}

	template<typename T>
	AssetRef<T> ForceLoad(AssetRef<AssetLoader> loader) {
		loader->AsyncLoad();
		loader->SyncLoad(m_assets);
		return Get<T>(loader->GetName());
	}
};

static AssetManager* GetAssetManager() { return AssetManager::GetInstance(); }