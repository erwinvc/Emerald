#pragma once

enum TileType : int8 {
	EMPTY = -1,
	FULL,
	INNER,
	OUTER,
	SLOPE,
	VALLEY
};

enum TileTransform {
	HIGH,
	LOW,
	UP,
	RIGHT,
	DOWN,
	LEFT
};

class Tile {
public:
	TileType m_type;
	byte m_materialID;
	TileTransform m_transformIndex;

	Tile() : m_type(FULL), m_transformIndex(HIGH) {}
	Tile(TileType type) : m_type(type), m_transformIndex(HIGH) {}
	Tile(int type) : m_type(TileType(type)), m_transformIndex(HIGH) {}

	void SetType(TileType type, TileTransform transform) {
		m_type = type;
		m_transformIndex = transform;
	}
};