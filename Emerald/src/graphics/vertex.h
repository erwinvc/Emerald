#pragma once

struct Vertex {
	glm::vec3 m_position;
	glm::vec3 m_normal;
	glm::vec2 m_uv;
	glm::vec3 m_tangent;
	glm::vec3 m_biTangent;

	Vertex() : m_position(glm::vec3()), m_normal(glm::vec3()), m_uv(glm::vec2()), m_tangent(glm::vec3()), m_biTangent(glm::vec3()) {}
};