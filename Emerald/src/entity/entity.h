#pragma once
class Entity {
private:

public:
	AssetRef<Model> m_model;
	Transform m_transform;

	Entity() {}
	Entity(AssetRef<Model> model) : m_model(model) {}
	virtual ~Entity() {}

	void Draw(Shader* overrideShader = nullptr, uint mode = GL_TRIANGLES) {
		//#Dirty
		if (overrideShader) {
			overrideShader->Bind();
			overrideShader->Set("_TransformationMatrix", m_transform.GetTransformationMatrix());
		} else {
			m_model->GetMaterials()[0]->Bind();
			m_model->GetMaterials()[0]->GetShader()->Set("_TransformationMatrix", m_transform.GetTransformationMatrix());
		}
		m_model->Draw(mode);
	}

	void OnImGui() {
		//m_transform.OnImGui();
	}
};

