#pragma once

class CustomLoader : public AssetLoader {
private:
	void(*m_async)();
	void(*m_sync)();

public:
	CustomLoader(const String& name, void(*async)(), void(*sync)() = Utils::nullfunc) : AssetLoader(name, true), m_async(async), m_sync(sync) {}

	void AsyncLoad() override {
		m_async();
	}

	void SyncLoad(map<String, AssetBase*>& assets) override {
		m_sync();
	}

	float GetProgress() override {
		return 0;
	}
};