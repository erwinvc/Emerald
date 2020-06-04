#include "stdafx.h"

glm::mat4 Transform::GetTransformationMatrix() {
	glm::mat4 matrix = glm::toMat4(GetOrientation()) * glm::translate(glm::mat4(1.0f), -position);
	matrix = glm::inverse(matrix) * glm::scale(size);

	//if (m_dirty) {
	//	glm::t
	//	m_cachedTransformationMatrix = glm::mat4::Identity();
	//	m_cachedTransformationMatrix = GetRotationMatrix() * glm::scale(m_size);
	//	m_cachedTransformationMatrix(3, 0) = m_position.x;
	//	m_cachedTransformationMatrix(3, 1) = m_position.y;
	//	m_cachedTransformationMatrix(3, 2) = m_position.z;
	//	m_dirty = false;
	//}

	//glm::mat4 myModelMatrix = glm::translate(glm::mat4(1.0f), m_position) * glm::toMat4(GetOrientation()) * glm::scale(m_size);
	return matrix;
}

glm::vec3 Transform::Forward() {
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
}


glm::vec3 Transform::Up() {
	return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
}


glm::vec3 Transform::Right() {
	return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
}

glm::quat Transform::GetOrientation() {
	return glm::quat(glm::vec3(-rotation.x, -rotation.y, -rotation.z));
}


float Transform::XMin() { return position.x - size.x / 2; }
float Transform::XMax() { return position.x + size.x / 2; }
float Transform::YMin() { return position.y - size.y / 2; }
float Transform::YMax() { return position.y + size.y / 2; }

bool Transform::Compare(const Transform& other) {
	return position == other.position && size == other.size && rotation == other.rotation;
}

GameObject* Transform::GetGameObject() {
	return m_gameObject;
}

void to_json(nlohmann::json& j, const Transform& obj) {
	//j = nlohmann::json{ {"position", obj.m_position}, {"size", obj.m_size}, {"rotation", obj.m_rotation} };
}

void from_json(const nlohmann::json& j, Transform& obj) {
	//j.at("position").get_to(obj.m_position);
	//j.at("size").get_to(obj.m_size);
	//j.at("rotation").get_to(obj.m_rotation);
}