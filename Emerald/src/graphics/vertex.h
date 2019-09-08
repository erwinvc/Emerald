#pragma once

struct Vertex {
	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_uv;
	Vector3 m_tangent;
	Vector3 m_biTangent;

	Vertex() : m_position(Vector3()), m_normal(Vector3()), m_uv(Vector2()), m_tangent(Vector3()), m_biTangent(Vector3()) {}
};