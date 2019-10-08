#include "stdafx.h"

void TileTextureLoader::SyncLoad(AssetManager* manager) {
	GetTileMaterialManager()->AddTexture(m_name, m_albedo, m_normal, m_roughness, m_metallic, m_emission);
	LOG("[~gTexture~x] Loaded ~1%s", m_name.c_str());
}