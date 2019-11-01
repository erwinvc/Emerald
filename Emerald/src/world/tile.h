#pragma once

class Tile {
private:
	Vector2I m_position;
	bool m_exists;
public:
	int m_textureID;

	Tile(Vector2I position, int textureID) : m_position(position), m_exists(true), m_textureID(textureID) {}
	Tile() : m_position(Vector2I()), m_exists(false), m_textureID(0) {}

	bool Exists() { return m_exists; }

	void SetTextureID(int textureID) {
		m_textureID = textureID;
		m_exists = true;
	}

	friend void to_json(nlohmann::json& j, const Tile& t) {
		j = nlohmann::json({ "textureID", t.m_textureID });
	}
};