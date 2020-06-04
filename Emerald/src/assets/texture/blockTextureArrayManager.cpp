#include "stdafx.h"

BlockTextureArrayManager::~BlockTextureArrayManager() {
	DELETE(m_albedoArray);
	DELETE(m_normalArray);
	DELETE(m_roughnessArray);
	DELETE(m_metallicArray);
	DELETE(m_emissionArray);
}

void BlockTextureArrayManager::Initialize() {
	m_albedoArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_SRGB, DATA_UNK, NEAREST)));
	m_normalArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RGB, DATA_UNK, LINEAR)));
	m_roughnessArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RED, DATA_UNK, NEAREST)));
	m_metallicArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RED, DATA_UNK, NEAREST)));
	m_emissionArray = NEW(TextureArray(TEXTURESIZE, LAYERCOUNT, TextureParameters(INT_RGB, DATA_UNK, NEAREST)));
}

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

int BlockTextureArrayManager::Get(const String& name) {
	auto found = m_textureIndices.find(name);
	if (found != m_textureIndices.end()) {
		return m_textureIndices[name];
	}
	LOG_ERROR("[~gTexture~x] Tile texture index ~1%s~x not found", name.c_str());
	return -1;
}

void BlockTextureArrayManager::GenerateMipmaps() {
	m_albedoArray->GenerateMipmaps();
	m_normalArray->GenerateMipmaps();
	m_roughnessArray->GenerateMipmaps();
	m_metallicArray->GenerateMipmaps();
	m_emissionArray->GenerateMipmaps();
}

void BlockTextureArrayManager::Bind() {
	m_albedoArray->Bind(0);
	m_normalArray->Bind(1);
	m_roughnessArray->Bind(2);
	m_metallicArray->Bind(3);
	m_emissionArray->Bind(4);
}