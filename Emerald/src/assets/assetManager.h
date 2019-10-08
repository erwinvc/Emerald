#pragma once

class AssetManager : public Singleton<AssetManager> {
	Timer m_timer;

	map<String, map<String, AssetBase*>> m_assets;
	AssetRef<AssetBatch> m_currentBatch;
	const int THREADCOUNT = 4;

	AssetManager() {
		//for (int i = 0; i < THREADCOUNT; i++)
		//	GetThreadManager()->RegisterThread(Format("AssetManagerPool%d", i), [] {});
	}
	~AssetManager() {
		for (auto it = m_assets.begin(); it != m_assets.end(); it++) {
			for (auto it2 = it->second.begin(); it2 != it->second.end(); it2++) {
				DELETE(it2->second);
			}
		}
	}
	friend Singleton;

public:

	template<typename T>
	AssetRef<T> CreateBatch(const String& name) {
		//TODO register batches
		LOG("[~yAssets~x] Asset batch ~1%s~x created", name.c_str());
		return NEW(T(name));
	}

	void SubmitBatch(AssetBatch* batch) {
		LOG("[~yAssets~x] Asset batch ~1%s~x submitted", batch->GetName().c_str());
		m_currentBatch = batch;
		m_currentBatch->Start();
	}

	void Update() {
		if (m_currentBatch) {
			m_currentBatch->Update(this);
			m_currentBatch->AsyncUpdate();
		}
	}

	float GetProgress() {
		if (!m_currentBatch) return 0;
		return m_currentBatch->GetProgress();
	}

	template<typename T>
	void AddAsset(const String& name, T* asset) {
		const String& typeName = T::GetAssetTypeName();
		if (m_assets[typeName][name] != nullptr) LOG_ERROR("[~yAssets~x] ~1%s~x already exists!", name.c_str());
		m_assets[typeName][name] = asset;
	}

	template<typename T>
	AssetRef<T> Get(const String& name) {
		const String& typeName = T::GetAssetTypeName();
		T* asset = (T*)m_assets[typeName][name];
		//if (asset == nullptr) LOG_WARN("[~yAssets~x] asset ~1%s~x of type ~1%s~x not found", name.c_str(), typeid(T).name());
		return asset;
	}

	template<typename T>
	AssetRef<T> ForceLoad(AssetRef<AssetLoader> loader) {
		loader->AsyncLoad();
		loader->SyncLoad(this);
		return Get<T>(loader->GetName());
	}
};

static AssetManager* GetAssetManager() { return AssetManager::GetInstance(); }