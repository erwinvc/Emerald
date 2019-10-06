#pragma once

class Tile
{
private:
	Vector2I m_position;
	int m_textureID;
public:
	Tile(Vector2I position, int textureID) : m_position(position), m_textureID(textureID) {}
	Tile() : m_position(Vector2I()), m_textureID(0){}
};