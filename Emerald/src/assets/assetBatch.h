#pragma once

class AssetBatch {
protected:
	String m_name;

	AssetBatch(const String& name) : m_name(name) {}

public:
	virtual ~AssetBatch() {}

	const String& GetName() { return m_name; }

	virtual void Add(AssetLoader* loader) = 0;
	virtual void Load(map<String, AssetBase*>& assets) = 0;
	virtual bool IsFinished() = 0;
	virtual float GetProgress() = 0;
};