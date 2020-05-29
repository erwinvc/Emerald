#include "stdafx.h"

void BlockTextureArrayManager::AddTexture(const String& name, const LoadedTextureArrayTexture& albedo, const LoadedTextureArrayTexture& normal, const LoadedTextureArrayTexture& roughness, const LoadedTextureArrayTexture& metallic, const LoadedTextureArrayTexture& emission) {
	int i1 = m_albedoArray->AddTexture(albedo);
	int i2 = m_normalArray->AddTexture(normal);
	int i3 = m_roughnessArray->AddTexture(roughness);
	int i4 = m_metallicArray->AddTexture(metallic);
	int i5 = m_emissionArray->AddTexture(emission);
	if ((i1 == i2) && (i1 == i3) && (i1 == i4) && (i1 == i5)) {
		m_textureIndices[name] = i1;
	} else LOG_ERROR("[~gTexture~x] Texture array registration index mismatch! %d %d %d %d %d", i1, i2, i3, i4, i5);
}