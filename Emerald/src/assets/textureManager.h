#pragma once

class TextureManager {
private:
    Texture* m_nullTexture;
    map<String, Texture*> m_textures;
public:
    void Initialize() {
        Byte bytes[] = { 0, 0, 0, -8, -8, 8, -8, -8,-8, 8, -8, -8, 0, 0, 0, -8 };
        uint texID;
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        m_nullTexture = new Texture(texID, 2, 2);
        LOG("[Textures] TextureManager initialized");
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