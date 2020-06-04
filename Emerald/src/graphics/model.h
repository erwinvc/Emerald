#pragma once

static bool MeshSort(AssetRef<Mesh>& mesh1, AssetRef<Mesh>& mesh2) { return (mesh1->GetMaterial()->GetID() < mesh2->GetMaterial()->GetID()); }

class Model : public AssetBase {
private:
	vector<AssetRef<Mesh>> m_meshes;
	String m_name;
	String m_dir;


public:
	Model(vector<AssetRef<Mesh>> meshes) : m_meshes(meshes) {
		sort(m_meshes.begin(), m_meshes.end(), MeshSort);
	}

	Model(AssetRef<Mesh> mesh) : m_meshes({ mesh }) {}
	Model() {}
	~Model() {}

	inline vector<AssetRef<Mesh>> GetMeshes() { return m_meshes; }

	void SetMaterial(AssetRef<Material> material);
	void DrawShadow(Shader* shadowShader, glm::mat4 transform, uint mode = GL_TRIANGLES);
	void DrawShadow(Shader* shadowShader, uint mode = GL_TRIANGLES);
	void Draw(glm::mat4 transform, uint mode = GL_TRIANGLES);
	void Draw(uint mode = GL_TRIANGLES);
	void DrawIndex(int index);

	static const String GetAssetTypeName() { return "Model"; }
};