#pragma once
class Entity {
private:

public:
	AssetRef<Model> m_model;
	Vector3 m_position;
	Vector3 m_rotation;
	Vector3 m_scale;

	Entity() : m_position(Vector3(0, 0, 0)), m_rotation(Vector3(0, 0, 0)), m_scale(Vector3(1, 1, 1)) {}
	Entity(AssetRef<Model> model) : m_position(Vector3(0, 0, 0)), m_rotation(Vector3(0, 0, 0)), m_scale(Vector3(1, 1, 1)), m_model(model) {}
	virtual ~Entity() {}

	void Draw(AssetRef<Shader> shader, uint mode = GL_TRIANGLES) {
		Matrix4 transform = Matrix4::TRS(m_position, m_rotation, m_scale);
		shader->Set("_TransformationMatrix", transform);
		m_model->Draw(shader, mode);
	}

	void OnImGui() {
		ImGui::DragFloat3("Position", (float*)&m_position, 0.1f);
		ImGui::DragFloat3("Rotation", (float*)&m_rotation, 0.1f);
		ImGui::DragFloat3("Scale", (float*)&m_scale, 0.1f);
	}
};

