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
	Vector2I m_position;
	const TileDefinition* m_definition;
	TileType m_type;
	TileTransform m_transformIndex;
	vector<Tile*> m_tiles;
	Tile* m_parent = nullptr;

	Tile() : m_position(Vector2I()), m_definition(&TileDefinition::SOLID), m_type(FULL), m_transformIndex(UP) {}
	Tile(Vector2I& pos) : m_position(pos), m_definition(&TileDefinition::SOLID), m_type(FULL), m_transformIndex(UP) {}
	Tile(const TileDefinition* definition, Vector2I& pos) : m_position(pos), m_definition(definition), m_type(FULL), m_transformIndex(UP) {}
	//Tile(TileType type) : m_definition(definition), m_type(type), m_transformIndex(UP) {}
	//Tile(int type) : m_definition(definition), m_type(TileType(type)), m_transformIndex(UP) {}

	void SetDefinition(const TileDefinition* definition) {
		m_definition = definition;
	}

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
		m_transformIndex = UP;
	}

	void SetFull() {
		m_type = FULL;
		m_transformIndex = UP;
	}

	inline bool IsSolid() { return m_type != GROUND; }
};