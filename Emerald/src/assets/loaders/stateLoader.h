#pragma once

class State;

class StateLoader : public AssetLoader {
private:
	State* m_state;
public:
	StateLoader(State* state);

	void AsyncLoad() override {};
	void SyncLoad(map<String, Asset*>& assets) override;

	float GetProgress() override {
		return 0;
	}
};