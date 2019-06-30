#pragma once

class TextureManager {
private:
	Texture* m_nullTexture;
	map<String, Texture*> m_textures;
public:
	void Initialize() {
		//#TODO Use texture class
		byte bytes[] = { (byte)0, (byte)0, (byte)0, (byte)-8, (byte)-8, (byte)8, (byte)-8, (byte)-8,(byte)-8, (byte)8, (byte)-8, (byte)-8, (byte)0, (byte)0, (byte)0, (byte)-8 };

		m_nullTexture = NEW(Texture(2, 2, bytes, TextureParameters(RGBA, NEAREST, REPEAT)));
		LOG("[~gTexture~x] Texture manager initialized");
	}
	void AddTexture(String name, Texture* texture) {
		m_textures.emplace(name, texture);
	}
	Texture* GetTexture(String texture) {
		return m_textures.at(texture);
	}

	~TextureManager() {
		map<String, Texture*>::iterator it;
		for (it = m_textures.begin(); it != m_textures.end(); it++) {
			DELETE(it->second);
			m_textures.erase(it);
		}
		DELETE(m_nullTexture);
	}

	Texture* GetNullTexture() {
		return m_nullTexture;
	}
};

TextureManager* GetTextureManager();