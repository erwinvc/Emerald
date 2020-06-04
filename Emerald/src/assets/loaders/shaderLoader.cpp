#include "stdafx.h"

void ShaderLoader::AsyncLoad() {}
void ShaderLoader::SyncLoad(AssetManager* manager) {
	GetShaderManager()->Create(m_name, m_file.GetFullPath(), m_hasGeometry, m_hasTessellation);
}

float ShaderLoader::GetProgress() {
	return 0;
}