#pragma once

class Rect {
public:
	Vector2 m_position;
	Vector2 m_size;
	Rect() : m_position(Vector2()), m_size(Vector2()) {}
	Rect(float x, float y, float w, float h) : m_position(Vector2(x, y)), m_size(Vector2(w, h)) {}

	Vector4& GetCornerPositions() const {
		return Vector4(m_position - m_size / 2, m_position + m_size / 2);
	}
};