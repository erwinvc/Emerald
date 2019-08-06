#pragma once

class AssetLoader {
protected:
	String m_name;
	bool m_shouldFinishSync = false;
	bool m_finishedAsync = false;
public:
	AssetLoader(const String& name, bool shouldFinishAsync) : m_name(name), m_shouldFinishSync(shouldFinishAsync) {}

	const String& GetName() { return m_name; }

	bool IsAsyncLoaded() { return m_finishedAsync; }

	virtual void AsyncLoad() = 0;
	virtual void SyncLoad(map<String, AssetBase*>& assets) = 0;

	virtual float GetProgress() = 0;
};