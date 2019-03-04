#pragma once

enum TileType{
    FULL,
    INNER,
    OUTER,
    SLOPE,
    VALLEY
};

class Tile {
public:
    TileType m_type;

    Tile(TileType type) : m_type(type) {}
};