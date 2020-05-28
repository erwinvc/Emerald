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

	vector<AssetRef<Mesh>> GetMeshes() { return m_meshes; }

	void SetMaterial(AssetRef<Material> material) {
		for (auto& mesh : m_meshes) {
			mesh->SetMaterial(material);
		}
	}

	void DrawShadow(uint mode = GL_TRIANGLES) {
		for (auto& mesh : m_meshes) {
			mesh->Draw(mode);
		}
	}
	
	void Draw(glm::mat4 transform, uint mode = GL_TRIANGLES) {
		m_meshes[0]->GetMaterial()->GetShader()->Set("_TransformationMatrix", transform);

		for (auto& mesh : m_meshes) {
			mesh->GetMaterial()->Bind();
			mesh->Draw(mode);
		}
	}

	void DrawIndex(int index) {
		m_meshes[index]->GetMaterial()->Bind();
		m_meshes[index]->Draw();
	}

	static const String GetAssetTypeName() { return "Model"; }
};