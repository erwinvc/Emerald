#pragma once

enum class MaterialCallbackType {
	ONBIND,
	ONINSTANCE
};

class Material;
class TextureArray;
class MaterialCallback {
protected:
	String m_uniform;
	uint m_uniformLocation;

	virtual void SetUniformLocation(Shader* shader) {
		m_uniformLocation = shader->GetUniformLocation(m_uniform.c_str());
	}

	friend Material;

public:
	MaterialCallback(const String& uniform) : m_uniform(uniform) {

	}
	virtual ~MaterialCallback() {}
	virtual void Callback() = 0;
};

template<typename T>
class MaterialCallbackPtr : public MaterialCallback {
private:
	T* m_ptr;
	uint m_size;

public:
	MaterialCallbackPtr(const String& uniform, T* ptr, uint size = 1) : MaterialCallback(uniform), m_ptr(ptr), m_size(size) {
	}

	void Callback() override {
		ShaderUniformBuffer::SetGL(m_uniformLocation, m_ptr, m_size);
	}
};

class MaterialCallbackTexture : public MaterialCallback {
private:
	Texture* m_texture;
	uint m_slot;
public:
	MaterialCallbackTexture(const String& uniform, Texture* texture, uint slot) : MaterialCallback(uniform), m_texture(texture), m_slot(slot) {}

	void Callback() override;
};

class MaterialCallbackTextureArray : public MaterialCallback {
private:
	TextureArray* m_texture;
	uint m_slot;
public:
	MaterialCallbackTextureArray(const String& uniform, TextureArray* texture, uint slot) : MaterialCallback(uniform), m_texture(texture), m_slot(slot) {}

	void Callback() override;
};
class MaterialCallbackGBuffer : public MaterialCallback {
private:
	uint m_uniformLocation1;
	uint m_uniformLocation2;
	uint m_uniformLocation3;

	void SetUniformLocation(Shader* shader) override {
		m_uniformLocation = shader->GetUniformLocation("_Depth");
		m_uniformLocation1 = shader->GetUniformLocation("_GAlbedo");
		m_uniformLocation2 = shader->GetUniformLocation("_GNormal");
		m_uniformLocation3 = shader->GetUniformLocation("_GMisc");
	}

public:
	MaterialCallbackGBuffer() : MaterialCallback("") {}

	void Callback() override;
};