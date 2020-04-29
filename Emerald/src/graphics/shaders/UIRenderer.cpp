#include "stdafx.h"

//void UIRenderer::Initialize() {
//	m_quad = MeshGenerator::Quad();
//	m_shader = GetShaderManager()->Get("UI");
//	OnResize(GetApp()->GetWidth<uint>(), GetApp()->GetHeight<uint>());
//}
//
//UIRenderer::~UIRenderer() {
//	DELETE(m_quad);
//}
//
//
//glm::mat4 UIRenderer::CreateMatrix(glm::vec2& origin, float rot, float positionX, float positionY, float sizeX, float sizeY, bool flipX, bool flipY) {
//	glm::mat4 transform = glm::translate(glm::mat4(1.0f), -glm::vec3(origin.x, origin.y, 0));
//	transform = glm::translate(transform, glm::vec3(positionX, positionY, 0));
//	transform = glm::rotate(transform, rot, glm::vec3(0.0f, 0.0f, 1.0f));
//	transform = glm::scale(transform, glm::vec3(glm::vec2((flipX ? -sizeX : sizeX) / 2, (flipY ? -sizeY : sizeY) / 2), 0));
//	transform = glm::translate(transform, glm::vec3(origin.x, origin.y, 0));
//	return transform;
//}
//
//void UIRenderer::RenderTexture(Texture* texture, glm::vec2 origin, float x, float y, float w, float h, float rotation = 0, const Color& color = Color::White(), const glm::vec3 atlasValues = glm::vec3(1, 0, 0)) {
//	m_shader->Bind();
//	texture->Bind(0); 
//
//	m_shader->Set("projectionMatrix", m_projection);
//	m_shader->Set("transformationMatrix", CreateMatrix(origin, rotation, x, y, w, h, false, false));
//	m_quad->Draw();
//}
//
//void UIRenderer::Rect(glm::vec2& origin, float positionX, float positionY, float sizeX, float sizeY, const Color& color, float rot) {
//	RenderTexture(GetTextureManager()->GetNullTexture(), origin, positionX, positionY, sizeX, sizeY, rot, color);
//}
//
//void UIRenderer::Rect(glm::vec2& origin, glm::vec2& position, glm::vec2& size, const Color& color, float rot) {
//	RenderTexture(GetTextureManager()->GetNullTexture(), origin, position.x, position.y, size.x, size.y, rot, color);
//}
//
//void UIRenderer::OnResize(uint width, uint height) {
//	m_projection = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);
//
//}