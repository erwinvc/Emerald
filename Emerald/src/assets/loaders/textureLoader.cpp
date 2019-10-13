#include "stdafx.h"

void TextureLoader::AsyncLoad() {
	TextureUtils::LoadTexture(m_file, m_params.GetFlipY(), [this](const LoadedTexture& data) {
		m_channelCount = data.m_channelCount;
		m_width = data.m_width;
		m_height = data.m_height;
		int size = m_height * m_width * 4;
		m_data = new byte[size];
		memcpy(m_data, data.m_data, size);

		//m_params.SetFormatFromChannelCount(data.m_channelCount); //STBImage already converts to RGBA so we don't need to do that
	});
}
void TextureLoader::SyncLoad(AssetManager* manager) {
	if (m_data != nullptr) {
		manager->AddAsset<Texture>(m_name, NEW(Texture(m_width, m_height, m_data, m_mipmaps, m_params)));
		LOG("[~gTexture~x] Loaded ~1%s", m_file.c_str());
	} else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", m_file.c_str());
}