#pragma once

class Ray {
private:
	glm::vec3 m_start;
	glm::vec3 m_previous;
	glm::vec3 m_end;
	glm::vec3 m_direction;

public:
	Ray(const glm::vec3& startPosition, const glm::vec3& rotation) : m_start(startPosition), m_previous(startPosition), m_end(startPosition) {
		m_direction = Utils::RotationToDirection(rotation);
	}

	void Step(float stepSize) {
		m_previous = m_end;
		m_end += m_direction * stepSize;
	}

	float GetLength() const {
		return glm::length(m_end - m_start);
	}

	const glm::vec3& GetEndpoint() const {
		return m_end;
	}

	const glm::vec3& GetLastPoint() const {
		return m_previous;
	}
};