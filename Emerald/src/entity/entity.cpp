#include "stdafx.h"

void Entity::Draw(uint mode) {
	m_model->Draw(transform.GetTransformationMatrix(), mode);
}

void Entity::DrawShadow(Shader* shadowShader, uint mode) {
	m_model->DrawShadow(shadowShader, transform.GetTransformationMatrix(), mode);
}