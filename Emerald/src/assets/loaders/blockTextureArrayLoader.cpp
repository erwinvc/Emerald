#include "stdafx.h"

void BlockTextureArrayLoader::SyncLoad(AssetManager* manager) {
	if (m_albedo.data == nullptr || m_normal.data == nullptr || m_roughness.data == nullptr || m_metallic.data == nullptr || m_emission.data == nullptr) {
		LOG_WARN("[~gTexture~x] failed to loaded ~1%s", m_name.c_str());
		return;
	}
	GetTileMaterialManager()->AddTexture(m_name, m_albedo, m_normal, m_roughness, m_metallic, m_emission);
	LOG("[~gTexture~x] loaded ~1%s", m_name.c_str());
}