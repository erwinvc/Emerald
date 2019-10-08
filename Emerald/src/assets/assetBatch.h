#pragma once

class AssetManager;
class AssetBatch {
protected:
	String m_name;

	AssetBatch(const String& name) : m_name(name) {}

public:
	virtual ~AssetBatch() {}

	const String& GetName() { return m_name; }

	virtual void Start() = 0;
	virtual void Add(AssetLoader* loader) = 0;
	virtual void Update(AssetManager* manager) = 0;
	virtual void AsyncUpdate() = 0;
	virtual bool IsFinished() = 0;
	virtual float GetProgress() = 0;
};