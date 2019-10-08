#pragma once

class State;

class StateLoader : public AssetLoader {
private:
	State* m_state;
public:
	StateLoader(State* state);

	void AsyncLoad() override {};
	void SyncLoad(AssetManager* manager) override;

	float GetProgress() override {
		return 0;
	}
};