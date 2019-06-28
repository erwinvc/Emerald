#include "stdafx.h"

void UIShader::Initialize() {
	m_quad = MeshGenerator::Quad();
}

UIShader::~UIShader() {
	DELETE(m_quad);
	DELETE(m_shader);
}


Matrix4 UIShader::CreateMatrix(Vector2& origin, float rot, Vector2& position, Vector2& scale, bool flipX, bool flipY) {
	Matrix4 transform = Matrix4::Identity();
	transform *= Matrix4::Translate(Vector3(-origin.x, -origin.y, 0));
	transform *= Matrix4::Translate(Vector3((position.x), (position.y), 0));
	transform *= Matrix4::Rotate(rot, Vector3::ZAxis());
	transform *= Matrix4::Scale(Vector2(flipX ? -scale.x : scale.x, flipY ? -scale.y : scale.y));
	transform *= Matrix4::Translate(Vector3(origin.x, origin.y, 0));


	//Matrix4 transform = Matrix4::Identity();
	//transform *= Matrix4::Rotate(rot, Vector3::ZAxis());
	//transform *= Matrix4::Scale(Vector2(flipX ? -scale.x : scale.x, flipY ? -scale.y : scale.y));
	return transform;
}

void UIShader::RenderTexture(uint texture, Vector2& origin, float rot, Vector2& position, Vector2& size, bool flipX, bool flipY) {
	m_shader->Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	m_shader->Set("textureSampler", 0);
	m_shader->Set("transformationMatrix", CreateMatrix(origin, rot, position, size, flipX, flipY));
	m_quad->Draw();
}