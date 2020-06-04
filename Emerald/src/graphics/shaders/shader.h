#pragma once

class Shader {
private:
	ShaderProgram* m_shaderProgram;

	bool m_hasGeometry;
	bool m_hasTessellation;
	String m_name;
	Path m_filePath;

	ShaderUniformBuffer m_uniformBuffer;
	ShaderProperties m_properties;

	Shader(const String& name, const Path& filePath, bool hasGeometry = false, bool hasTessellation = false);
	~Shader();
	
	GLuint LoadShader(const String& path, GLuint type);
	bool AddShaderToProgram(ShaderProgram* program, const String& shader, GLuint shaderType);

	ShaderProgram* Load();

	friend class ShaderManager;

public:
	static uint s_boundShader;
	
	inline ShaderProgram* GetShaderProgram() const { return m_shaderProgram; }
	inline const ShaderUniformBuffer* GetUniformBuffer() const { return &m_uniformBuffer; }
	inline const ShaderProperties* GetShaderProperties() const { return &m_properties; }
	inline const String& GetName() { return m_name; }

	inline uint GetUniformLocation(const String_t location) {
		return m_uniformBuffer.GetUniformLocation(location);
	}

	inline void SetFloat(uint location, const float value) {
		m_uniformBuffer.SetFloat(location, value);
	}

	inline void SetInt(uint location, const int value) {
		m_uniformBuffer.SetInt(location, value);
	}

	inline void SetBool(uint location, const bool value) {
		m_uniformBuffer.SetInt(location, value);
	}

	inline void SetVec2(uint location, const glm::vec2& value) {
		m_uniformBuffer.SetVec2(location, value);
	}

	inline void SetVec3(uint location, const glm::vec3& value) {
		m_uniformBuffer.SetVec3(location, value);
	}

	inline void SetVec4(uint location, const glm::vec4& value) {
		m_uniformBuffer.SetVec4(location, value);
	}

	inline void SetColor(uint location, const Color& value) {
		m_uniformBuffer.SetColor(location, value);
	}

	inline void SetMat4(uint location, const glm::mat4& value) {
		m_uniformBuffer.SetMat4(location, value);
	}
	
	template<typename T>
	void Set(const String_t location, const T* value, uint count) {
		m_uniformBuffer.Set(location, value, count);
	}

	template<typename T>
	void Set(const String_t location, const T& value) {
		m_uniformBuffer.Set(location, value);
	}

	template <>
	void Set<bool>(const String_t location, const bool& value) {
		m_uniformBuffer.Set(location, (int)value);
	}

	void Reload();
	void Bind();
	void Unbind();
	void OnImGUI();

	static const String GetAssetTypeName() { return "Shader"; }
};