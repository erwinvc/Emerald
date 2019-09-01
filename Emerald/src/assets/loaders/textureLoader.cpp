#include "stdafx.h"

void TextureLoader::AsyncLoad() {
	TextureUtils::LoadTexture(m_file, m_params.GetFlipY(), [this](byte* data, uint width, uint height) {
		m_width = width;
		m_height = height;
		int size = 4 * m_width * m_height;
		m_data = new byte[size];
		memcpy(m_data, data, size);
	});
}
void TextureLoader::SyncLoad(map<String, AssetBase*>& assets) {
	if (m_data != nullptr) {
		assets[m_name] = NEW(Texture(m_width, m_height, m_data, m_params));
		LOG("[~gTexture~x] Loaded ~1%s", m_file.c_str());
	} else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", m_file.c_str());
}