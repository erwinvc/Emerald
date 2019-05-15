#pragma once

class TextureManager {
private:
    Texture* m_nullTexture;
    map<String, Texture*> m_textures;
public:
    void Initialize() {
		//#TODO Use texture class
        Byte bytes[] = { 0, 0, 0, -8, -8, 8, -8, -8,-8, 8, -8, -8, 0, 0, 0, -8 };

		m_nullTexture = new Texture(2, 2, bytes, TextureParameters(RGBA, NEAREST, REPEAT));
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
            delete it->second;
            m_textures.erase(it);
        }
    }

    Texture* GetNullTexture() {
        return m_nullTexture;
    }
};

TextureManager* GetTextureManager();