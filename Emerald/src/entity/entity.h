#pragma once
class Entity {
private:

public:
	Model* m_model;
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

	Entity() {}
	Entity(Model* model) : m_model(model) {
		m_position = Vector3(0, 0, -1);
		m_rotation = Vector3();
		m_scale = 0.2f;
	}
	virtual ~Entity() {}

	void Draw(Shader* shader) {
		Matrix4 transform = Matrix4::TRS(m_position, m_rotation, m_scale);
		shader->Set("transformationMatrix", transform);
		m_model->Draw(shader);
	}

	void OnImGui() {
		ImGui::DragFloat3("Position", (float*)&m_position, 0.1f);
		ImGui::DragFloat3("Rotation", (float*)&m_rotation, 0.1f);
		ImGui::DragFloat3("Scale", (float*)&m_scale, 0.1f);
	}
};

