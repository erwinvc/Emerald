#include "stdafx.h"

void Sprite::Draw(HDRPipeline* pipeline, glm::vec2 origin, float x, float y, float w, float h, float rotation) {
	if (m_frameCount > 0) {
		int textureIndex;
		textureIndex = (int)(GetApp()->GetTotalFrameTime() / (1000.0f * m_frameTime)) % m_frameCount;
		textureIndex += m_frameOffset;
		float row = (float)(m_numberOfRows - (textureIndex / m_numberOfRows) - 1);
		float column = textureIndex % m_numberOfRows;
		m_atlasValues = glm::vec3(m_numberOfRows, column / m_numberOfRows, row / m_numberOfRows);
	}

	pipeline->Rect(origin, x, y, w, h, rotation, m_color, m_texture, m_atlasValues);
	//GetUIRenderer()->RenderTexture(transform.m_position.x, transform.m_position.y, transform.m_size.x, transform.m_size.y, transform.m_rotation, m_color, tex, m_atlasValues);
}