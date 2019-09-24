#include "stdafx.h"

void UIRenderer::Initialize() {
	m_quad = MeshGenerator::Quad();
	m_shader = GetShaderManager()->Get("UI");
}

UIRenderer::~UIRenderer() {
	DELETE(m_quad);
}


Matrix4 UIRenderer::CreateMatrix(Vector2& origin, float rot, float positionX, float positionY, float sizeX, float sizeY, bool flipX, bool flipY) {
	Matrix4 transform = Matrix4::Identity();
	transform *= Matrix4::Translate(Vector3(-origin.x, -origin.y, 0));
	transform *= Matrix4::Translate(Vector3(positionX, positionY, 0));
	transform *= Matrix4::Rotate(rot, Vector3::ZAxis());
	transform *= Matrix4::Scale(Vector2((flipX ? -sizeX : sizeX) / 2, (flipY ? -sizeY : sizeY) / 2));
	transform *= Matrix4::Translate(Vector3(origin.x, origin.y, 0));
	return transform;
}

void UIRenderer::RenderTexture(AssetRef<Texture> texture, Vector2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color, float rot, bool flipX, bool flipY) {
	m_shader->Bind();
	texture->Bind(0);
	m_shader->Set("_Texture", 0);
	m_shader->Set("_Color", color);

	Matrix4 projection = Matrix4::Orthographic(0, GetApp()->GetWidth(), 0, GetApp()->GetHeight(), -1.0f, 1.0f);
	m_shader->Set("projectionMatrix", projection);
	m_shader->Set("transformationMatrix", CreateMatrix(origin, rot, positionX, positionY, sizeX, sizeY, flipX, flipY));
	m_quad->Draw();
}

void UIRenderer::Rect(Vector2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color, float rot) {
	RenderTexture(GetTextureManager()->GetNullTexture(), origin, positionX, positionY, sizeX, sizeY, color, rot, false, false);
}

void UIRenderer::Rect(Vector2& origin, Vector2& position, Vector2& size, const Color& color, float rot) {
	RenderTexture(GetTextureManager()->GetNullTexture(), origin, position.x, position.y, size.x, size.y, color, rot, false, false);
}
void UIRenderer::RenderTexture(AssetRef<Texture> texture, Vector2& origin, Vector2& position, Vector2& size, const Color& color, float rot, bool flipX, bool flipY) {
	RenderTexture(texture, origin, rot, position.x, position.y, size.x, size.y, flipX, flipY);
}