#pragma once

class GameObject;
class Transform {
private:
	GameObject* m_gameObject;
	friend GameObject;
public:
	glm::vec3 m_position;
	glm::vec3 m_size;
	glm::vec3 m_rotation;
	glm::mat4 m_cachedTransformationMatrix;
	bool m_dirty = true;

	Transform() : m_gameObject(nullptr), m_position(glm::vec3(0.0f)), m_size(glm::vec3(1.0f)), m_rotation(0.0f) {}
	Transform(GameObject* gameObject) : m_gameObject(gameObject), m_position(glm::vec3(0.0f)), m_size(glm::vec3(1.0f)), m_rotation(0.0f) {}
	Transform(const Transform& other, GameObject* gameObject) : m_gameObject(gameObject), m_position(other.m_position), m_size(other.m_size), m_rotation(other.m_rotation) {}

	//Transform(const Transform& other) = delete;

	void SetDirty() {
		m_dirty = true;
	}

	glm::mat4 GetTransformationMatrix() {
		glm::mat4 matrix = glm::toMat4(GetOrientation()) * glm::translate(glm::mat4(1.0f), -m_position);
		matrix = glm::inverse(matrix) * glm::scale(m_size);
		
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

	glm::vec3 Forward() {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}


	glm::vec3 Up() {
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}


	glm::vec3 Right() {
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::quat GetOrientation() {
		return glm::quat(glm::vec3(-m_rotation.x, -m_rotation.y, -m_rotation.z));
	}


	float XMin() { return m_position.x - m_size.x / 2; }
	float XMax() { return m_position.x + m_size.x / 2; }
	float YMin() { return m_position.y - m_size.y / 2; }
	float YMax() { return m_position.y + m_size.y / 2; }

	bool Compare(const Transform& other) {
		return m_position == other.m_position && m_size == other.m_size && m_rotation == other.m_rotation;
	}

	//Rectangle AsRectangle() {
	//	return { m_position, m_size };
	//}

	//bool IsPointWithin(float x, float y) {
	//	return Math::Within(x, XMin(), XMax()) && Math::Within(y, YMin(), YMax());
	//}

	GameObject* GetGameObject() {
		return m_gameObject;
	}

	friend void to_json(nlohmann::json& jsonObject, const Transform& transform);

	friend void from_json(const nlohmann::json& jsonObject, Transform& transform);
};