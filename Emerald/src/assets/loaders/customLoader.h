#pragma once

class AssetManager;
class CustomLoader : public AssetLoader {
private:
	void(*m_async)();
	void(*m_sync)();

public:
	CustomLoader(const String& name, void(*async)(), void(*sync)() = Utils::nullfunc) : AssetLoader(name), m_async(async), m_sync(sync) {}

	void AsyncLoad() override {
		m_async();
	}

	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};