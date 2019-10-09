#pragma once

class AssetManager;
class AssetLoader {
protected:
	String m_name;

public:
	AssetLoader(const String& name) : m_name(name) {}
	virtual ~AssetLoader() {}
	const String& GetName() { return m_name; }

	virtual void AsyncLoad() = 0;
	virtual void SyncLoad(AssetManager* manager) = 0;

	virtual float GetProgress() = 0;
};