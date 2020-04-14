#pragma once

struct Rectangle {
	union {
		struct {
			glm::vec2 m_position;
			glm::vec2 m_size;
		};
		struct {
			float x, y, w, h;
		};

	};

	Rectangle() :x(0), y(0), w(0), h(0) {}
	Rectangle(float _x, float _y, float _w, float _h) : x(_x), y(_y), w(_w), h(_h) {}
	Rectangle(int _x, int _y, int _w, int _h) : x((float)_x), y((float)_y), w((float)_w), h((float)_h) {}
	Rectangle(glm::vec2& _position, glm::vec2& _size) : m_position(_position), m_size(_size) {}

	glm::vec4 GetCornerPositions() const {
		return { m_position - m_size / 2.0f, m_position + m_size / 2.0f};
	}

	bool Within(Rectangle& other);

	float XMin() { return m_position.x - m_size.x / 2; }
	float XMax() { return m_position.x + m_size.x / 2; }
	float YMin() { return m_position.y - m_size.y / 2; }
	float YMax() { return m_position.y + m_size.y / 2; }
};