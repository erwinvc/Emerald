#pragma once

enum TileType : int8 {
	GROUND,
	FULL,
	SINGLE,
	SINGLECONNECTION,
	SINGLECORNER,
	OUTER,
	SINGLESTRAIGHT,
	T,
	SLOPETOSINGLE1,
	SLOPETOSINGLE2,
	SLOPE,
	CROSS,
	OUTERTODOUBLE,
	TSOLID,
	VALLEY,
	INNER,
};

static String TileTypeToString(TileType type) {
	switch (type) {
	case GROUND: return "GROUND";
	case FULL: return "FULL";
	case SINGLE: return "SINGLE";
	case SINGLECONNECTION: return "SINGLECONNECTION";
	case SINGLECORNER: return "SINGLECORNER";
	case OUTER: return "OUTER";
	case SINGLESTRAIGHT: return "SINGLESTRAIGHT";
	case T: return "T";
	case SLOPETOSINGLE1: return "SLOPETOSINGLE1";
	case SLOPETOSINGLE2: return "SLOPETOSINGLE2";
	case SLOPE: return "SLOPE";
	case CROSS: return "CROSS";
	case OUTERTODOUBLE: return "OUTERTODOUBLE";
	case TSOLID: return "TSOLID";
	case VALLEY: return "VALLEY";
	case INNER: return "INNER";
	}
	return "NULL";
}

enum TileTransform {
	UP,
	RIGHT,
	DOWN,
	LEFT,
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
		m_type = GROUND;
	}

	void SetFull() {
		m_type = FULL;
	}

	inline bool IsRock() { return m_type != GROUND; }
};