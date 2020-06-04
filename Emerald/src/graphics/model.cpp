#include "stdafx.h"

void Model::SetMaterial(AssetRef<Material> material) {
	for (auto& mesh : m_meshes) {
		mesh->SetMaterial(material);
	}
}

void Model::DrawShadow(Shader* shadowShader, glm::mat4 transform, uint mode) {
	shadowShader->Bind();
	shadowShader->Set("_TransformationMatrix", transform);
	for (auto& mesh : m_meshes) {
		mesh->Draw(mode);
	}
}

void Model::DrawShadow(Shader* shadowShader, uint mode) {
	shadowShader->Bind();
	shadowShader->Set("_TransformationMatrix", glm::mat4(1));
	for (auto& mesh : m_meshes) {
		mesh->Draw(mode);
	}
}



void Model::Draw(glm::mat4 transform, uint mode) {
	m_meshes[0]->GetMaterial()->GetShader()->Bind();
	m_meshes[0]->GetMaterial()->GetShader()->Set("_TransformationMatrix", transform);

	for (auto& mesh : m_meshes) {
		mesh->GetMaterial()->Bind();
		mesh->Draw(mode);
	}
}

void Model::Draw(uint mode) {
	m_meshes[0]->GetMaterial()->GetShader()->Bind();
	m_meshes[0]->GetMaterial()->GetShader()->Set("_TransformationMatrix", glm::mat4(1));

	for (auto& mesh : m_meshes) {
		mesh->GetMaterial()->Bind();
		mesh->Draw(mode);
	}
}

void Model::DrawIndex(int index) {
	m_meshes[index]->GetMaterial()->Bind();
	m_meshes[index]->Draw();
}