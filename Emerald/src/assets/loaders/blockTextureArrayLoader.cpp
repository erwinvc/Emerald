#include "stdafx.h"

void BlockTextureArrayLoader::SyncLoad(AssetManager* manager) {
	if (m_albedo.data == nullptr || m_normal.data == nullptr || m_roughness.data == nullptr || m_metallic.data == nullptr || m_emission.data == nullptr) {
		LOG_WARN("[~gTexture~x] failed to loaded ~1%s", m_name.c_str());
		return;
	}
	GetTileMaterialManager()->AddTexture(m_name, m_albedo, m_normal, m_roughness, m_metallic, m_emission);
	LOG("[~gTexture~x] loaded ~1%s", m_name.c_str());
}

BlockTextureArrayLoader::~BlockTextureArrayLoader() {
	delete[] m_albedo.data;
	delete[] m_normal.data;
	delete[] m_roughness.data;
	delete[] m_metallic.data;
	delete[] m_emission.data;
}


void BlockTextureArrayLoader::LoadAsyncTexture(LoadedTextureArrayTexture& loadedTextureData, const String& type) {
	loadedTextureData.path = Format("res/%s_%s.png", m_file.c_str(), type.c_str());
	TextureUtils::LoadTexture(loadedTextureData.path, true, [&](const LoadedTexture& data) {
		loadedTextureData.params.SetDataFormatFromChannelCount(data.m_channelCount);
		int size = data.m_height * data.m_width * data.m_channelCount;
		loadedTextureData.size = glm::uvec2(data.m_width, data.m_height);
		loadedTextureData.data = new byte[size];
		memcpy(loadedTextureData.data, data.m_data, size);
	});
}

void BlockTextureArrayLoader::AsyncLoad() {
	LoadAsyncTexture(m_albedo, "albedo");
	LoadAsyncTexture(m_normal, "normal");
	LoadAsyncTexture(m_metallic, "metallic");
	LoadAsyncTexture(m_roughness, "roughness");
	LoadAsyncTexture(m_emission, "emission");
}

float BlockTextureArrayLoader::GetProgress() {
	return 0;
}