#include "stdafx.h"

BasicMaterial::BasicMaterial() :
	m_albedo(GetTextureManager()->GetNullTexture()),
	m_normal(GetTextureManager()->GetNormalTexture()),
	m_roughness(GetTextureManager()->GetWhiteTexture()),
	m_metallic(GetTextureManager()->GetBlackTexture()),
	m_emission(GetTextureManager()->GetTransparentTexture()) {
}

void BasicMaterial::SetPBR(const String& name) {
	SetAlbedo(GetAssetManager()->Get<Texture>(name + "_albedo"));
	SetNormal(GetAssetManager()->Get<Texture>(name + "_normal"));
	SetRoughness(GetAssetManager()->Get<Texture>(name + "_roughness"));
	SetMetallic(GetAssetManager()->Get<Texture>(name + "_metallic"));
	SetEmission(GetAssetManager()->Get<Texture>(name + "_emission"));
}

BasicMaterial::~BasicMaterial() {}