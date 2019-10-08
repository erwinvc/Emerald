#include "stdafx.h"

void ShaderLoader::SyncLoad(AssetManager* manager) {
	GetShaderManager()->Create(m_name, m_file, m_hasGeometry, m_hasTessellation);
}