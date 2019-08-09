#pragma once

enum TileType : int8 {
	EMPTY = -1,
	FULL,
	INNER,
	OUTER,
	SLOPE,
	VALLEY
};

static String TileTypeToString(TileType type) {
	switch (type) {
	case EMPTY: return "EMPTY";
	case FULL: return "FULL";
	case INNER: return "INNER";
	case OUTER: return "OUTER";
	case SLOPE: return "SLOPE";
	case VALLEY: return "VALLEY";
	}
	return "NULL";
}

enum TileTransform {
	UP,
	RIGHT,
	DOWN,
	LEFT,
	LOW,
	HIGH
};
static TileTransform RotateTileTransform(TileTransform transform, int times) {
	int trans = transform;
	trans += times;
	trans = trans % 4;
	return (TileTransform)trans;
}

static String TileTransformToString(TileTransform transform) {
	switch (transform) {
	case UP: return "UP";
	case RIGHT: return "RIGHT";
	case DOWN: return "DOWN";
	case LEFT: return "LEFT";
	}
	return "NULL";
}

class Tile {
public:
	TileType m_type;
	byte m_materialID;
	TileTransform m_transformIndex;
	//Vector4 m_heights;

	Tile() : m_type(FULL), m_transformIndex(UP) {}
	Tile(TileType type) : m_type(type), m_transformIndex(UP) {}
	Tile(int type) : m_type(TileType(type)), m_transformIndex(UP) {}

	void Set(TileType type, TileTransform transform) {
		m_type = type;
		m_transformIndex = transform;
	}

	void SetType(TileType type) {
		m_type = type;
	}

	void SetTransform(TileTransform transform) {
		m_transformIndex = transform;
	}

	void SetEmpty() {
		m_type = EMPTY;
		m_transformIndex = LOW;
	}

	void SetFull() {
		m_type = FULL;
		m_transformIndex = HIGH;
	}

	inline bool IsRock() { return m_type != EMPTY; }
};