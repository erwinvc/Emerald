#pragma once

class AssetManager : public Singleton<AssetManager> {
	Timer m_timer;

	map<String, map<String, AssetBase*>> m_assets;
	AssetRef<AssetBatch> m_currentBatch;
	const int THREADCOUNT = 4;

	AssetManager() {}
	~AssetManager();
	friend Singleton;

public:

	template<typename T>
	AssetRef<T> CreateBatch(const String& name) {
		//TODO register batches
		LOG("[~yResource~x] asset batch ~1%s~x created", name.c_str());
		return NEW(T(name));
	}

	void SubmitBatch(AssetBatch* batch);

	void Update();

	float GetProgress();

	template<typename T>
	void AddAsset(const String& name, T* asset) {
		const String& typeName = T::GetAssetTypeName();
		//if (m_assets[typeName][name] != nullptr) LOG_ERROR("[~yResource~x] ~1%s~x already exists!", name.c_str());
		if (m_assets[typeName][name] != nullptr) {
			DELETE(m_assets[typeName][name]);
			m_assets[typeName][name] = nullptr;
		}
		m_assets[typeName][name] = asset;
	}

	template<typename T>
	AssetRef<T> Get(const String& name, bool warn = true) {
		const String& typeName = T::GetAssetTypeName();
		T* asset = (T*)m_assets[typeName][name];
		if (warn && asset == nullptr) LOG_WARN("[~yResource~x] asset ~1%s~x of type ~1%s~x not found", name.c_str(), typeid(T).name());
		return asset;
	}

	template<typename T>
	AssetRef<T> ForceLoad(AssetLoader* loader) {
		loader->AsyncLoad();
		loader->SyncLoad(this);
		return Get<T>(loader->GetName());
	}
};

static AssetManager* GetAssetManager() { return AssetManager::GetInstance(); }