#include "stdafx.h"

BasicMaterial::BasicMaterial() :
	m_albedo(GetTextureManager()->GetNullTexture()),
	m_normal(GetTextureManager()->GetNormalTexture()),
	m_specular(GetTextureManager()->GetWhiteTexture()),
	m_metallic(GetTextureManager()->GetBlackTexture()),
	m_emission(GetTextureManager()->GetTransparentTexture()) {
}

BasicMaterial::~BasicMaterial() {}