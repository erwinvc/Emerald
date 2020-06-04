#include "stdafx.h"

PBRTextureLoader::~PBRTextureLoader() {
	delete[] m_albedo;
	delete[] m_normal;
	delete[] m_roughness;
	delete[] m_metallic;
	delete[] m_emission;
}

void PBRTextureLoader::AsyncLoad() {
	TextureUtils::LoadTexture(Format("res/%s_albedo%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
		int size = data.m_height * data.m_width * data.m_channelCount;
		m_sizes[0] = glm::vec2((float)data.m_width, (float)data.m_height);
		m_channelCounts[0] = data.m_channelCount;
		m_albedo = new byte[size];
		memcpy(m_albedo, data.m_data, size);
	});

	TextureUtils::LoadTexture(Format("res/%s_normal%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
		int size = data.m_height * data.m_width * data.m_channelCount;
		m_sizes[1] = glm::vec2((float)data.m_width, (float)data.m_height);
		m_channelCounts[1] = data.m_channelCount;
		m_normal = new byte[size];
		memcpy(m_normal, data.m_data, size);
	});

	TextureUtils::LoadTexture(Format("res/%s_roughness%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
		int size = data.m_height * data.m_width * data.m_channelCount;
		m_sizes[2] = glm::vec2((float)data.m_width, (float)data.m_height);
		m_channelCounts[2] = data.m_channelCount;
		m_roughness = new byte[size];
		memcpy(m_roughness, data.m_data, size);
	});

	TextureUtils::LoadTexture(Format("res/%s_metallic%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
		int size = data.m_height * data.m_width * data.m_channelCount;
		m_sizes[3] = glm::vec2((float)data.m_width, (float)data.m_height);
		m_channelCounts[3] = data.m_channelCount;
		m_metallic = new byte[size];
		memcpy(m_metallic, data.m_data, size);
	});

	TextureUtils::LoadTexture(Format("res/%s_emission%s", m_file.c_str(), m_extention.c_str()), 1, [this](const LoadedTexture& data) {
		int size = data.m_height * data.m_width * data.m_channelCount;
		m_sizes[4] = glm::vec2((float)data.m_width, (float)data.m_height);
		m_channelCounts[4] = data.m_channelCount;
		m_emission = new byte[size];
		memcpy(m_emission, data.m_data, size);
	});
}

float PBRTextureLoader::GetProgress() {
	return 0;
}

void PBRTextureLoader::SyncLoad(AssetManager* manager) {
	AddAssetToManager(manager, m_name + "_albedo", m_albedo, 0, TextureParameters(INT_SRGBA));
	AddAssetToManager(manager, m_name + "_normal", m_normal, 1, TextureParameters(INT_RGB));
	AddAssetToManager(manager, m_name + "_roughness", m_roughness, 2, TextureParameters(INT_RED));
	AddAssetToManager(manager, m_name + "_metallic", m_metallic, 3, TextureParameters(INT_RED));
	AddAssetToManager(manager, m_name + "_emission", m_emission, 4, TextureParameters(INT_RGB));
}

void PBRTextureLoader::AddAssetToManager(AssetManager* manager, const String& name, byte* data, int index, TextureParameters& params) {
	if (data != nullptr) {
		params.SetDataFormatFromChannelCount(m_channelCounts[index]);
		manager->AddAsset<Texture>(name, NEW(Texture((int32)m_sizes[index].x, (int32)m_sizes[index].y, data, true, params)));
	}
}