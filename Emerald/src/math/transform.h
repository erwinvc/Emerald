#pragma once

class GameObject;
class Transform {
private:
	GameObject* m_gameObject;
	friend GameObject;
public:
	glm::vec3 position;
	glm::vec3 size;
	glm::vec3 rotation;
	glm::mat4 m_cachedTransformationMatrix;
	bool m_dirty = true;

	Transform() : m_gameObject(nullptr), position(glm::vec3(0.0f)), size(glm::vec3(1.0f)), rotation(0.0f) {}
	Transform(GameObject* gameObject) : m_gameObject(gameObject), position(glm::vec3(0.0f)), size(glm::vec3(1.0f)), rotation(0.0f) {}
	Transform(const Transform& other, GameObject* gameObject) : m_gameObject(gameObject), position(other.position), size(other.size), rotation(other.rotation) {}

	inline void SetDirty() { m_dirty = true; }

	glm::mat4 GetTransformationMatrix();
	glm::vec3 Forward();
	glm::vec3 Up();
	glm::vec3 Right();
	glm::quat GetOrientation();

	float XMin();
	float XMax();
	float YMin();
	float YMax();

	bool Compare(const Transform& other);

	GameObject* GetGameObject();

	friend void to_json(nlohmann::json& jsonObject, const Transform& transform);
	friend void from_json(const nlohmann::json& jsonObject, Transform& transform);
};