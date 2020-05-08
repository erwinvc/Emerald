#include "stdafx.h"

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