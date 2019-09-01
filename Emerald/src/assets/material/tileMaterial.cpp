#include "stdafx.h"

void TileMaterial::Bind(AssetRef<Shader> shader) {
	shader->Set("_Albedo", 0);
	shader->Set("_Normal", 1);
	shader->Set("_Specular", 2);
	shader->Set("_Emission", 3);
	GetTileTextureManager()->GetAlbedo()->Bind(0);
	GetTileTextureManager()->GetNormal()->Bind(1);
	GetTileTextureManager()->GetSpecular()->Bind(2);
	GetTileTextureManager()->GetEmission()->Bind(3);
}
void TileMaterial::Unbind() {
	GetTileTextureManager()->GetAlbedo()->Unbind();
}