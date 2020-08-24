#pragma once

class Texture : public AssetBase {
private:
	TextureParameters m_params;
	uint m_textureID;
	int32 m_width, m_height;
	byte* m_data = nullptr;
	bool m_keepData = false;
	bool m_hasMipmaps;
	uint8 m_mipmapCount = 0;
	uint8 m_channelCount = 0;
	void Cleanup();

public:
	Texture(int32 width, int32 height, byte* data, bool hasMipmaps, TextureParameters params = TextureParameters(), bool keepData = false);
	Texture(int32 width, int32 height, bool hasMipmaps, TextureParameters params = TextureParameters(), bool keepData = false);
	~Texture();

	void SetData(byte* data);
	uint GetHandle() { return m_textureID; }
	void* GetImGuiHandle() { return (void*)(uint64)m_textureID; }
	void Bind(uint slot = 0);
	void Unbind(uint slot = 0);
	void Resize(uint width, uint height);

	inline const uint32 GetWidth() const { return m_width; }
	inline const uint32 GetHeight() const { return m_height; }
	inline byte* GetData() const { return m_data; }
	inline const TextureParameters& GetTextureParams() { return m_params; }
	inline uint8 GetChannelCount() const { return m_channelCount; }

	void SaveAsPNG(const String& path);
	static inline const String GetAssetTypeName() { return "Texture"; }
};