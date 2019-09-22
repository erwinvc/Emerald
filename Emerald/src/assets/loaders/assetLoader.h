#pragma once

class AssetLoader {
protected:
	String m_name;
	bool m_shouldFinishSync = false;
public:
	AssetLoader(const String& name, bool shouldFinishAsync) : m_name(name) {}

	const String& GetName() { return m_name; }

	virtual void AsyncLoad() = 0;
	virtual void SyncLoad(map<String, AssetBase*>& assets) = 0;

	virtual float GetProgress() = 0;
};