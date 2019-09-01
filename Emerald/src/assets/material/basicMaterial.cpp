#include "stdafx.h"

BasicMaterial::BasicMaterial() :
	m_albedo(GetTextureManager()->GetNullTexture()),
	m_normal(GetTextureManager()->GetNullTexture()),
	m_specular(GetTextureManager()->GetNullTexture()),
	m_emission(GetTextureManager()->GetNullTexture()) {
}

BasicMaterial::~BasicMaterial() {}