#pragma once

struct EntityState {
	glm::vec3 position{ 0.0f };
	glm::vec3 rotation{ 0.0f };

	glm::vec3 lastPosition{ 0.0f };
	glm::vec3 lastRotation{ 0.0f };

	glm::vec3 velocity{ 0.0 };

	float lastUpdateTime = 0;
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

	void Draw(Shader* overrideShader = nullptr, uint mode = GL_TRIANGLES) {
		//#Dirty
		if (overrideShader) {
			overrideShader->Bind();
			overrideShader->Set("_TransformationMatrix", transform.GetTransformationMatrix());
		}
		m_model->Draw(transform.GetTransformationMatrix(), mode);
	}

	void DrawShadow(Shader* shadowShader, uint mode = GL_TRIANGLES) {
		shadowShader->Set("_TransformationMatrix", transform.GetTransformationMatrix());
		m_model->DrawShadow(mode);
	}

	void OnImGui() {
		//m_transform.OnImGui();
	}
};

