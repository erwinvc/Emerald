#pragma once

struct LoadedTextureArrayTexture;
class TextureArray {
	TextureParameters m_params;
	uint m_handle;
	uint m_index;
	uint m_size;
	uint m_layers;

public:
	TextureArray(uint size, uint layers, TextureParameters params);

	int AddTexture(const LoadedTextureArrayTexture& textureData);
	void GenerateMipmaps();
	void Bind(uint slot = 0) const;
	void Unbind() const;
};