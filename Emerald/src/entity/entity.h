#pragma once
class Entity {
private:

public:
	AssetRef<Model> m_model;
	Transform m_transform;

	Entity() {}
	Entity(AssetRef<Model> model) : m_model(model) {}
	virtual ~Entity() {}

	void Draw(AssetRef<Shader> shader, uint mode = GL_TRIANGLES) {
		shader->Set("_TransformationMatrix", m_transform.GetMatrix());
		m_model->Draw(shader, mode);
	}

	void OnImGui() {
		m_transform.OnImGui();
	}
};

