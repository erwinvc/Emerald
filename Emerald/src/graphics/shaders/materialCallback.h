#pragma once

enum class MaterialCallbackType {
	ONBIND,
	ONINSTANCE
};

class Material;
class MaterialCallback {
protected:
	String m_uniform;
	uint m_uniformLocation;

	void SetUniformLocation(Shader* shader) {
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

public:
	MaterialCallbackPtr(const String& uniform, T* ptr) : MaterialCallback(uniform), m_ptr(ptr) {}

	void Callback() override {
		ShaderUniformBuffer::SetGL(m_uniformLocation, m_ptr, 1);
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