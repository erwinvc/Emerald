#include "stdafx.h"
#include "graphics/buffers/indexBuffer.h"

SpriteRenderer::SpriteRenderer() {
	uint32* indicesBuffer = new uint32[INDICES_SIZE];

	uint32 offset = 0;
	for (int32 i = 0; i < INDICES_SIZE; i += 6) {
		indicesBuffer[i] = offset + 0;
		indicesBuffer[i + 1] = offset + 1;
		indicesBuffer[i + 2] = offset + 2;

		indicesBuffer[i + 3] = offset + 2;
		indicesBuffer[i + 4] = offset + 3;
		indicesBuffer[i + 5] = offset + 0;
		offset += 4;
	}
	m_shader = GetShaderManager()->Get("UI");
	m_shader->Bind();
	uint32 textures[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31 };
	m_shader->Set("_Textures", textures, 32);

	Mesh* mesh = new Mesh(NEW(VertexArray()), new IndexBuffer(indicesBuffer, INDICES_SIZE), GetMaterialManager()->GetBasicMaterial(m_shader));
	m_elementsRenderer = new ElementsRenderer<Vertex>(mesh, VERTEX_COUNT, INDICES_COUNT, MAX_SPRITES, m_layout);

	delete mesh;
	delete[] indicesBuffer;
}

void SpriteRenderer::Rect(glm::vec2 origin, float x, float y, float w, float h, float rotation, const Color& color, Texture* texture, const glm::vec3& atlasValues) {
	float textureSlot = 0.0f;
	if (texture) textureSlot = SubmitTexture(texture);
	Vertex vertices[4];

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), -glm::vec3(origin.x, origin.y, 0));
	transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
	transform = glm::rotate(transform, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, glm::vec3(w / 2.0f, h / 2.0f, 0));
	transform = glm::translate(transform, glm::vec3(origin.x, origin.y, 0));

	//Top left
	vertices[0].m_position = glm::vec3(transform * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0));
	vertices[0].m_uv = glm::vec2(0.0f, 1.0f);
	vertices[0].m_textureID = textureSlot;
	vertices[0].m_color = color;
	vertices[0].vsAtlasValues = atlasValues;
	//Top right
	vertices[1].m_position = glm::vec3(transform * glm::vec4(1.0f, 1.0f, 0.0f, 1.0));
	vertices[1].m_uv = glm::vec2(1.0f, 1.0f);
	vertices[1].m_textureID = textureSlot;
	vertices[1].m_color = color;
	vertices[1].vsAtlasValues = atlasValues;
	//Bottom right
	vertices[2].m_position = glm::vec3(transform * glm::vec4(1.0f, -1.0f, 0.0f, 1.0));
	vertices[2].m_uv = glm::vec2(1.0f, 0.0f);
	vertices[2].m_textureID = textureSlot;
	vertices[2].m_color = color;
	vertices[2].vsAtlasValues = atlasValues;
	//Bottom left
	vertices[3].m_position = glm::vec3(transform * glm::vec4(-1.0f, -1.0f, 0.0f, 1.0));
	vertices[3].m_uv = glm::vec2(0.0f, 0.0f);
	vertices[3].m_textureID = textureSlot;
	vertices[3].m_color = color;
	vertices[3].vsAtlasValues = atlasValues;

	m_elementsRenderer->Submit(&vertices[0], VERTEX_COUNT, 1);
}

void SpriteRenderer::Line(float x0, float y0, float x1, float y1, Color& color, float size) {
	float textureSlot = 0.0f;
	glm::vec2 normal = glm::normalize(glm::vec2(y1 - y0, -(x1 - x0))) * glm::vec2(size, size);
	Vertex vertices[4];


	vertices[2].m_position = glm::vec3(x0 + normal.x, y0 + normal.y, 0.0f);
	//vertices[0].m_position = glm::vec3(x0 + normal.x, y0 + normal.y, 0.0f);
	vertices[0].m_uv = glm::vec2(0.0f, 1.0f);
	vertices[0].m_textureID = textureSlot;
	vertices[0].m_color = color;
	vertices[0].vsAtlasValues = glm::vec3(1, 0, 0);

	vertices[1].m_position = glm::vec3(x1 + normal.x, y1 + normal.y, 0.0f);
	//vertices[1].m_position = glm::vec3(1920.0f, 1080.0f, 0.0f);
	vertices[1].m_uv = glm::vec2(1.0f, 1.0f);
	vertices[1].m_textureID = textureSlot;
	vertices[1].m_color = color;
	vertices[1].vsAtlasValues = glm::vec3(1, 0, 0);

	vertices[0].m_position = glm::vec3(x1 - normal.x, y1 - normal.y, 0.0f);
	//vertices[2].m_position = glm::vec3(1920.0f, 0.0f, 0.0f);
	vertices[2].m_uv = glm::vec2(1.0f, 0.0f);
	vertices[2].m_textureID = textureSlot;
	vertices[2].m_color = color;
	vertices[2].vsAtlasValues = glm::vec3(1, 0, 0);

	vertices[3].m_position = glm::vec3(x0 - normal.x, y0 - normal.y, 0.0f);
	//vertices[3].m_position = glm::vec3(0.0f, 0.0f, 0.0f);
	vertices[3].m_uv = glm::vec2(0.0f, 0.0f);
	vertices[3].m_textureID = textureSlot;
	vertices[3].m_color = color;
	vertices[3].vsAtlasValues = glm::vec3(1, 0, 0);

	m_elementsRenderer->Submit(&vertices[0], VERTEX_COUNT, 1);
}

void SpriteRenderer::LineRect(Rectangle& rect, Color& color, float lineSize) {
	float halfLineSize = lineSize / 2;
	Line(rect.XMin() - halfLineSize, rect.YMin() - halfLineSize, rect.XMin() - halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
	Line(rect.XMax() + halfLineSize, rect.YMin() - halfLineSize, rect.XMax() + halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
	Line(rect.XMin() - halfLineSize, rect.YMin() - halfLineSize, rect.XMax() + halfLineSize, rect.YMin() - halfLineSize, color, lineSize);
	Line(rect.XMin() - halfLineSize, rect.YMax() + halfLineSize, rect.XMax() + halfLineSize, rect.YMax() + halfLineSize, color, lineSize);
}

void SpriteRenderer::Draw() {
	m_shader->Bind();
	m_shader->Set("_View", Camera::uiActive->GetViewMatrix());
	m_shader->Set("_Projection", Camera::uiActive->GetProjectionMatrix());

	for (uint32 i = 0; i < m_textures.size(); i++) {
		m_textures[i]->Bind(i);
	}
	m_elementsRenderer->Draw();
}

void SpriteRenderer::Begin() {
	m_textures.clear();
	SubmitTexture(GetTextureManager()->GetWhiteTexture());
	m_elementsRenderer->Begin();
}

void SpriteRenderer::End() {
	m_elementsRenderer->End();
}

float SpriteRenderer::SubmitTexture(Texture* texture) {
	float result = 0.0f;
	bool found = false;
	for (uint i = 0; i < m_textures.size(); i++) {
		if (m_textures[i] == texture) {
			result = (float)(i);
			found = true;
			break;
		}
	}

	if (!found) {
		if (m_textures.size() >= 31) {
			//#TODO: Flush
		}
		m_textures.push_back(texture);
		result = (float)(m_textures.size() - 1);
	}
	return result;
}