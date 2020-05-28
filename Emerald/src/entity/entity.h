#pragma once
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

