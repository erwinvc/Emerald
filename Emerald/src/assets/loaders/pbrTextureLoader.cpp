#include "stdafx.h"

void PBRTextureLoader::SyncLoad(AssetManager* manager) {
	AddAssetToManager(manager, m_name + "_albedo", m_albedo, 0);
	AddAssetToManager(manager, m_name + "_normal", m_normal, 1);
	AddAssetToManager(manager, m_name + "_roughness", m_roughness, 2);
	AddAssetToManager(manager, m_name + "_metallic", m_metallic, 3);
	AddAssetToManager(manager, m_name + "_emission", m_emission, 4);
}

void PBRTextureLoader::AddAssetToManager(AssetManager* manager, const String& name, byte* data, int index) {
	if (data != nullptr) {
		LOG("Added %s", name.c_str());
		manager->AddAsset<Texture>(name, NEW(Texture((int32)m_sizes[index].x, (int32)m_sizes[index].y, data, true, TextureParameters())));
	}
}