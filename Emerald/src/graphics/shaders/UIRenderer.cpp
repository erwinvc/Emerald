#include "stdafx.h"

void UIRenderer::Initialize() {
	m_quad = MeshGenerator::Quad();
	m_shader = GetShaderManager()->Get("UI");
}

UIRenderer::~UIRenderer() {
	DELETE(m_quad);
}


Matrix4 UIRenderer::CreateMatrix(Vector2& origin, float rot, Vector2& position, Vector2& scale, bool flipX, bool flipY) {
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

void UIRenderer::Rect(Vector2& origin, Vector2& position, Vector2& size, const Color& color, float rot) {
	RenderTexture(GetTextureManager()->GetNullTexture(), origin, position, size, color, rot, false, false);
}
void UIRenderer::RenderTexture(AssetRef<Texture> texture, Vector2& origin, Vector2& position, Vector2& size, const Color& color, float rot, bool flipX, bool flipY) {
	RenderTexture(texture->GetHandle(), origin, position, size, color, rot, flipX, flipY);
}

void UIRenderer::RenderTexture(uint texture, Vector2& origin, Vector2& position, Vector2& size, const Color& color, float rot, bool flipX, bool flipY) {
	m_shader->Bind();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	m_shader->Set("_Texture", 0);
	m_shader->Set("_Color", color);

	Matrix4 projection = Matrix4::Orthographic(0, GetApplication()->GetWidth(), 0, GetApplication()->GetHeight(), -1.0f, 1.0f);
	m_shader->Set("projectionMatrix", projection);
	m_shader->Set("transformationMatrix", CreateMatrix(origin, rot, position, size, flipX, flipY));
	m_quad->Draw();
}