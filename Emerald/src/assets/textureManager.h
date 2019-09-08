#pragma once

class TextureManager {
private:
	Texture* m_nullTexture;
	Texture* m_whiteTexture;
	Texture* m_blackTexture;
	Texture* m_normalTexture;
	map<String, AssetRef<Texture>> m_textures;
public:
	void Initialize() {
		//#TODO Use texture class

		Color8 nullColors[] = {
			Color::Black().ToColor8(),
			Color::Magenta().ToColor8(),
			Color::Magenta().ToColor8(),
			Color::Black().ToColor8()
		};

		m_nullTexture = NEW(Texture(2, 2, (uint8*)nullColors, TextureParameters(RGBA, RGBA, NEAREST, REPEAT)));
		m_whiteTexture = NEW(Texture(1, 1, (uint8*)Color::White().ToColor8(), TextureParameters(RGBA, RGBA, NEAREST, REPEAT)));
		m_blackTexture = NEW(Texture(1, 1, (uint8*)Color::Black().ToColor8(), TextureParameters(RGBA, RGBA, NEAREST, REPEAT)));
		m_normalTexture = NEW(Texture(1, 1, (uint8*)Color::NormalMap().ToColor8(), TextureParameters(RGBA, RGBA, NEAREST, REPEAT)));
		LOG("[~gTexture~x] Texture manager initialized");
	}
	void AddTexture(String name, Texture* texture) {
		m_textures.emplace(name, texture);
	}
	AssetRef<Texture> GetTexture(String texture) {
		return m_textures.at(texture);
	}

	~TextureManager() {
		DELETE(m_nullTexture);
		DELETE(m_whiteTexture);
		DELETE(m_blackTexture);
		DELETE(m_normalTexture);
	}

	AssetRef<Texture> GetNullTexture() { return m_nullTexture; }
	AssetRef<Texture> GetWhiteTexture() { return m_whiteTexture; }
	AssetRef<Texture> GetBlackTexture() { return m_blackTexture; }
	AssetRef<Texture> GetNormalTexture() { return m_normalTexture; }
};

TextureManager* GetTextureManager();