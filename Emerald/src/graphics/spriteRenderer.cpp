#include "stdafx.h"

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