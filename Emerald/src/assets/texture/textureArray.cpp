#include "stdafx.h"

int TextureArray::AddTexture(const LoadedTextureArrayTexture& textureData) {
	if (textureData.size.x != m_size || textureData.size.y != m_size) LOG_ERROR("[~gTexture~x] ~1%s~x texture size (%u %u) does not match texture array size (%u %u)", textureData.path.GetFileName().c_str(), textureData.size.x, textureData.size.y, m_size, m_size);
	GL(glBindTexture(GL_TEXTURE_2D_ARRAY, m_handle));
	GL(glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, m_index, m_size, m_size, 1, textureData.params.GetDataFormat(), m_params.GetType(), textureData.data));
	return m_index++;
}