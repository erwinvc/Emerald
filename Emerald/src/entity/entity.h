#pragma once
class Entity {
private:

public:
	Mesh m_mesh;
	Vector3 m_position;
	Vector3 m_rotation;
	float m_scale;

    Entity() {}
	Entity(Mesh& mesh) : m_mesh(mesh)
	{
		m_position = Vector3(0, 0, -1);
		m_rotation = Vector3();
		m_scale = 0.2f;
	}
	virtual ~Entity() {}

	void Draw(Shader& shader)
	{

		Matrix4 transform = Matrix4::Identity();
		transform *= Matrix4::Translate(m_position);
		transform *= Matrix4::Rotate(m_rotation.x, Vector3::XAxis());
		transform *= Matrix4::Rotate(m_rotation.y, Vector3::YAxis());
		transform *= Matrix4::Rotate(m_rotation.z, Vector3::ZAxis());
		transform *= Matrix4::Scale(m_scale);

		//Matrix4 projection = Matrix4::Perspective(65.0f, 1.778f, 0.01f, 1000.0f);
		//Matrix4 model = Matrix4::Translate(m_position) * Matrix4::Rotate(m_rotation.x, Vector3(1, 0, 0)) * Matrix4::Rotate(m_rotation.y, Vector3(0, 1, 0)) * Matrix4::Rotate(m_rotation.z, Vector3(0, 0, 1)) * Matrix4::Scale(Vector3(m_scale));
		//Matrix4 mvp = projection * model;

		shader.Set("transformationMatrix", transform);
		m_mesh.Draw();
	}
};

