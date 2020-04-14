#include "stdafx.h"

void UIRenderer::Initialize() {
	m_quad = MeshGenerator::Quad();
	m_shader = GetShaderManager()->Get("UI");
}

UIRenderer::~UIRenderer() {
	DELETE(m_quad);
}


glm::mat4 UIRenderer::CreateMatrix(glm::vec2& origin, float rot, float positionX, float positionY, float sizeX, float sizeY, bool flipX, bool flipY) {
	//glm::mat4 transform = glm::mat4::Identity();
	//transform *= glm::mat4::Translate(glm::vec3(-origin.x, -origin.y, 0));
	//transform *= glm::mat4::Translate(glm::vec3(positionX, positionY, 0));
	//transform *= glm::mat4::Rotate(rot, glm::vec3::ZAxis());
	//transform *= glm::mat4::Scale(glm::vec2((flipX ? -sizeX : sizeX) / 2, (flipY ? -sizeY : sizeY) / 2));
	//transform *= glm::mat4::Translate(glm::vec3(origin.x, origin.y, 0));
	return glm::mat4(1.0f);
}

void UIRenderer::RenderTexture(AssetRef<Texture> texture, glm::vec2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color, float rot, bool flipX, bool flipY) {
	m_shader->Bind();
	texture->Bind(0);
	m_shader->Set("_Texture", 0);
	m_shader->Set("_Color", color);

	//glm::mat4 projection = glm::mat4::Orthographic(0.0f, GetApp()->GetWidth<float>(), 0.0f, GetApp()->GetHeight<float>(), -1.0f, 1.0f);
	//m_shader->Set("projectionMatrix", projection);
	m_shader->Set("transformationMatrix", CreateMatrix(origin, rot, positionX, positionY, sizeX, sizeY, flipX, flipY));
	m_quad->Draw();
}

void UIRenderer::Rect(glm::vec2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color, float rot) {
	RenderTexture(GetTextureManager()->GetNullTexture(), origin, positionX, positionY, sizeX, sizeY, color, rot, false, false);
}

void UIRenderer::Rect(glm::vec2& origin, glm::vec2& position, glm::vec2& size, const Color& color, float rot) {
	RenderTexture(GetTextureManager()->GetNullTexture(), origin, position.x, position.y, size.x, size.y, color, rot, false, false);
}
void UIRenderer::RenderTexture(AssetRef<Texture> texture, glm::vec2& origin, glm::vec2& position, glm::vec2& size, const Color& color, float rot, bool flipX, bool flipY) {
	RenderTexture(texture, origin, rot, position.x, position.y, size.x, size.y, flipX, flipY);
}