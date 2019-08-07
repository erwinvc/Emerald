#pragma once

enum TileType : uint8 {
	FULL,
	INNER,
	OUTER,
	SLOPE,
	VALLEY
};

enum Rotation {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

class Tile {
public:
	TileType m_type;
	byte m_materialID;
	int rotationValue;

	Tile() : m_type(FULL) {}
	Tile(TileType type) : m_type(type) {}
	Tile(int type) : m_type(TileType(type)) {}
};