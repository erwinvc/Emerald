#pragma once

class TextureManager : public Singleton<TextureManager> {
private:
	Texture* m_nullTexture;
	Texture* m_whiteTexture;
	Texture* m_blackTexture;
	Texture* m_normalTexture;
	Texture* m_transparentTexture;
	map<String, AssetRef<Texture>> m_textures;
	
public:
	~TextureManager();
	
	void Initialize();
	void AddTexture(String name, Texture* texture);
	AssetRef<Texture> GetTexture(const String& texture);

	inline AssetRef<Texture> GetNullTexture() { return m_nullTexture; }
	inline AssetRef<Texture> GetWhiteTexture() { return m_whiteTexture; }
	inline AssetRef<Texture> GetBlackTexture() { return m_blackTexture; }
	inline AssetRef<Texture> GetNormalTexture() { return m_normalTexture; }
	inline AssetRef<Texture> GetTransparentTexture() { return m_transparentTexture; }
};

inline TextureManager* GetTextureManager() { return TextureManager::GetInstance(); }