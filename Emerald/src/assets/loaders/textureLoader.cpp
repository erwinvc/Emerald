#include "stdafx.h"
#include "../stb_image.h"

void TextureLoader::AsyncLoad() {
	if (!FileSystem::DoesFileExist(m_file)) {
		LOG_ERROR("[~gTexture~x] file at ~1%s~x does not exist!", m_file.c_str());
		return;
	}

	int bpc;

	stbi_set_flip_vertically_on_load(1);
	m_data = stbi_load(m_file.c_str(), (int*)&m_width, (int*)&m_height, &bpc, 4);

	if (bpc != 3 && bpc != 4) {
		LOG_ERROR("[~gTexture~x] Unsupported image bit-depth (%d) ~1%s", bpc, m_file.c_str());
		return;
	}

	m_finishedAsync = true;
}
void TextureLoader::SyncLoad(map<String, Asset*>& assets) {
	if (m_data != nullptr) {
		Texture* texture = NEW(Texture(m_width, m_height, m_data, m_params));
		stbi_image_free(m_data);
		assets.emplace(m_name, texture);
		LOG("[~gTexture~x] Loaded ~1%s", m_file.c_str());
	} else LOG_ERROR("[~gTexture~x] Failed to load ~1%s", m_file.c_str());
}