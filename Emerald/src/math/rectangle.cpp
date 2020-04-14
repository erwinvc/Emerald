#include "stdafx.h"

//Checks if this rect is within other rect
bool Rectangle::Within(Rectangle& other) {
	bool withinX = Math::Within(m_position.x - m_size.x / 2, m_position.x + m_size.x / 2, other.m_position.x - other.m_size.x / 2, other.m_position.x + other.m_size.x / 2);
	if (!withinX) return false;
	bool withinY = Math::Within(m_position.y - m_size.y / 2, m_position.y + m_size.y / 2, other.m_position.y - other.m_size.y / 2, other.m_position.y + other.m_size.y / 2);
	if (!withinY) return false;
	return true;
}