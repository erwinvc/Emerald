#pragma once

class TileManager : public Singleton<TileManager> {
private:
	const int MAXTILEDEFINITIONS = 256;
	TileDefinition* m_tileDefinitions[MAXTILEDEFINITIONS] = { 0 };
	int m_definitionIndex = 0;

	void ParseJsonData() {
		FileSystem::CreateFileIfDoesntExist("tileDefinitions.json");
		JsonWrapper jsonOb = FileSystem::LoadJsonFromFile("tileDefinitions");
		
	}

public:

	void Initialize() {
		ParseJsonData();
	}

	int RegisterTileDefinition(TileDefinition* definition) {
		if (m_definitionIndex >= MAXTILEDEFINITIONS) LOG_ERROR("[Tiles] tile definition index exceeded %d", MAXTILEDEFINITIONS);
		m_tileDefinitions[m_definitionIndex++] = definition;
	}

	inline AssetRef<TileDefinition> GetTileDefinition(int index) { return m_tileDefinitions[index]; }
};

static TileManager* GetTileManager() { return TileManager::GetInstance(); }