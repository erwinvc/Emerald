#pragma once

struct EntityState {
	char name[16];
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };

	glm::vec3 lastPosition{ 0.0f };
	glm::vec3 lastRotation{ 0.0f };

	glm::vec3 velocity{ 0.0 };

	bool active = false;
};

class Entity {
private:

public:
	AssetRef<Model> m_model;
	Transform transform;

	Entity() : transform(Transform()) {}
	Entity(AssetRef<Model> model) : m_model(model) {}
	virtual ~Entity() {}

	void Draw(uint mode = GL_TRIANGLES);
	void DrawShadow(Shader* shadowShader, uint mode = GL_TRIANGLES);

	void OnImGui() {
	}
};

