#pragma once

struct Vertex {
	Vector3 m_position;
	Vector3 m_normal;
	Vector2 m_uv;
	Vector3 m_tangents;
	Vector3 m_biTangents;

	Vertex() : m_position(Vector3()), m_normal(Vector3()), m_uv(Vector2()), m_tangents(Vector3()), m_biTangents(Vector3()) {}
};