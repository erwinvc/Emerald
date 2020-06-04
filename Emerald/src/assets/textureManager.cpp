#include "stdafx.h"

TextureManager::~TextureManager() {
	DELETE(m_nullTexture);
	DELETE(m_whiteTexture);
	DELETE(m_blackTexture);
	DELETE(m_normalTexture);
}

void TextureManager::Initialize() {
	Color8 nullColors[] = {
		Color::Black().ToColor8(),
		Color::Magenta().ToColor8(),
		Color::Magenta().ToColor8(),
		Color::Black().ToColor8()
	};

	m_nullTexture = NEW(Texture(2, 2, (uint8*)nullColors, false, TextureParameters(INT_RGBA, DATA_RGBA, NEAREST, REPEAT)));
	m_whiteTexture = NEW(Texture(1, 1, (uint8*)Color::White().ToColor8(), false, TextureParameters(INT_RGBA, DATA_RGBA, NEAREST, REPEAT)));
	m_blackTexture = NEW(Texture(1, 1, (uint8*)Color::Black().ToColor8(), false, TextureParameters(INT_RGBA, DATA_RGBA, NEAREST, REPEAT)));
	m_normalTexture = NEW(Texture(1, 1, (uint8*)Color::NormalMap().ToColor8(), false, TextureParameters(INT_RGBA, DATA_RGBA, NEAREST, REPEAT)));
	m_transparentTexture = NEW(Texture(1, 1, (uint8*)Color::Transparent().ToColor8(), false, TextureParameters(INT_RGBA, DATA_RGBA, NEAREST, REPEAT)));
	LOG("[~gTexture~x] Texture manager initialized");
}
void TextureManager::AddTexture(String name, Texture* texture) {
	m_textures.emplace(name, texture);
}
AssetRef<Texture> TextureManager::GetTexture(const String& texture) {
	return m_textures.at(texture);
}