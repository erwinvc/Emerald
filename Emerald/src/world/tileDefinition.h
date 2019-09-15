#pragma once

class TileDefinition {

private:
	//bool m_isEmpty = false;
	String m_name;
	uint8 m_materialID;

public:
	TileDefinition(const String& name, uint8 materialID) : m_name(name), m_materialID(materialID) {}

	const String& GetName() const { return m_name; }
	const uint8 GetMaterialID() const { return m_materialID; }

	static TileDefinition ANY;
	static TileDefinition SOLID;
	static TileDefinition SOLID2;
	static TileDefinition EMPTY;
	static TileDefinition ROOM;
	static TileDefinition HOOK;
	static TileDefinition LOCK;
};