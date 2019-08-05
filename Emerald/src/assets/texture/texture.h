#pragma once

class Texture : public Asset {
private:
	TextureParameters m_params;
	uint m_textureID = 0;
	int32 m_width, m_height;
	String m_path;
	byte* m_data = nullptr;
	bool m_keepData;

public:
	Texture(int32 width, int32 height, byte* data, TextureParameters params = TextureParameters(), bool keepData = false);
	Texture(int32 width, int32 height, TextureParameters params = TextureParameters());
	Texture(const String& path, TextureParameters params = TextureParameters(), bool keepData = false);
	Texture(uint textureID, uint width, uint height, TextureParameters params = TextureParameters()) : m_params(params), m_textureID(textureID), m_width(width), m_height(height) {}
	~Texture();

	void SetData(byte* data);

	uint GetHandle() { return m_textureID; }
	void Bind(uint slot = 0);
	void Unbind(uint slot = 0);


	inline const int32 GetWidth() const { return m_width; }
	inline const int32 GetHeight() const { return m_height; }
	inline byte* GetData() const { return m_data; }
};