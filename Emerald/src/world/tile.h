#pragma once

enum TileType : uint8 {
    FULL,
    INNER,
    OUTER,
    SLOPE,
    VALLEY
};

class Tile {
public:
    TileType m_type;
	byte m_materialID;

	Tile() : m_type(FULL) {}
	Tile(TileType type) : m_type(type) {}
	Tile(int type) : m_type(TileType(type)) {}
};