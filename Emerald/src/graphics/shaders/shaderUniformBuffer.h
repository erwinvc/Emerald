#pragma once

class ShaderProgram;

enum class ShaderUniformType {
	INT,
	FLOAT,
	VEC2,
	VEC3,
	VEC4,
	//IVEC2,
	//IVEC3,
	//IVEC4,
	//MAT3,
	MAT4,
};

class ShaderUniform {
private:
	String m_name;

	uint m_size;
	uint m_count;
	uint m_offset;
	uint m_location;

	ShaderUniformType m_type;

public:
	bool m_locked = false;
	bool m_existsInShader;
	bool m_firstUpload = true;

	ShaderUniform(const String& name, ShaderUniformType type, uint size, uint offset, uint count, uint location, bool existsInShader)
		: m_name(name), m_size(size), m_offset(offset), m_type(type), m_count(count), m_location(location), m_existsInShader(existsInShader) {
	}

	const String& GetName() { return m_name; }
	uint GetSize() { return m_size; }
	uint GetCount() { return m_count; }
	uint GetOffset() { return m_offset; }
	uint GetLocation() { return m_location; }
	ShaderUniformType GetType() { return m_type; }
};

class ShaderUniformBuffer {
private:
	map<String, ShaderUniform> m_uniforms;
	byte* m_data = nullptr;	//All data stored in one buffer
	uint m_offset = 0;		//Memory offset in bytes. Total size when all uniform have been added
	bool m_allocated = false;

	static uint ShaderUniformTypeToSize(ShaderUniformType type);
	static ShaderUniformType GLTypeToShaderUniformType(GLenum type);
	bool SetUniformLocal(uint offset, void* value, uint size, uint count);
	void SetUniformGL(ShaderUniform& uniform);
	void Allocate();
	void DeAllocate();

public:
	~ShaderUniformBuffer();

	void AddUniform(const String& name, ShaderUniformType type, uint location, uint count, bool existsInShader);
	void AddUniform(const String& name, GLenum glType, uint location, uint count, bool existsInShader);

	static void SetGL(uint location, const int value) { glUniform1iv(location, 1, (int*)&value); }
	static void SetGL(uint location, const int* value, uint count) { glUniform1iv(location, count, (int*)value); }
	static void SetGL(uint location, const glm::ivec2* value, uint count) { glUniform2iv(location, count, (int*)value); }
	static void SetGL(uint location, const float* value, uint count) { glUniform1fv(location, count, (float*)value); }
	static void SetGL(uint location, const glm::vec2* value, uint count) { glUniform2fv(location, count, (float*)value); }
	static void SetGL(uint location, const glm::vec3* value, uint count) { glUniform3fv(location, count, (float*)value); }
	static void SetGL(uint location, const glm::vec4* value, uint count) { glUniform4fv(location, count, (float*)value); }
	static void SetGL(uint location, const Color* value, uint count) { glUniform4fv(location, count, (float*)value); }
	static void SetGL(uint location, const glm::mat4* value, uint count) { glUniformMatrix4fv(location, count, GL_FALSE, (float*)value); }

	void SetFloat(uint location, const float value) {
		SetGL(location, &value, 1);
	}
	void SetInt(uint location, const int& value) {
		SetGL(location, (int*)&value, 1);
	}
	void SetVec2(uint location, const glm::ivec2& value) {
		SetGL(location, &value, 1);
	}
	void SetVec3(uint location, const glm::vec2& value) {
		SetGL(location, &value, 1);
	}
	void SetVec4(uint location, const glm::vec3& value) {
		SetGL(location, &value, 1);
	}
	void SetColor(uint location, const Color& value) {
		SetGL(location, &value, 1);
	}
	void SetMat4(uint location, const glm::mat4& value) {
		SetGL(location, &value, 1);
	}

	template<typename T>
	void Set(const String_t name, const T* value, uint count) {
		ASSERT(m_allocated, "[~bShaders~x] ShaderUniformBuffer memory needs to be allocated first!");

		auto it = m_uniforms.find(name);
		if (it == m_uniforms.end()) return;
		ShaderUniform& uniform = it->second;
		if (uniform.m_locked || !uniform.m_existsInShader) return;
		uint size = uniform.GetSize();
		uint offset = uniform.GetOffset();
		uint location = uniform.GetLocation();
		uint valueSize = sizeof(T) * count;
		uint expectedSize = uniform.GetSize() * uniform.GetCount();
		ASSERT(valueSize == expectedSize, "[~bShaders~x] ~1%s~x data size mismatch. Got ~1%d~x out of ~1%d~x bytes. Does the name match the uniform?", name, valueSize, expectedSize);

		if (!SetUniformLocal(uniform.GetOffset(), (void*)value, size, uniform.GetCount()) || uniform.m_firstUpload) {
			SetUniformGL(uniform);
			uniform.m_firstUpload = false;
		}
	}


	template<typename T>
	void Set(const String_t name, const T& value) {
		Set<T>(name, &value, 1);
	}

	uint GetUniformLocation(const String_t location);
	void RegisterUniforms(ShaderProgram* shaderProgram, map<String, ShaderUniform>* oldMap = nullptr);
	void Reload(ShaderProgram* shaderProgram);

	const map<ShaderUniformType, vector<ShaderUniform>> GetUniforms() {
		map<ShaderUniformType, vector<ShaderUniform>> m_typeMap;
		for (auto& [name, uniform] : m_uniforms) {
			m_typeMap[uniform.GetType()].push_back(uniform);
		}

		return m_typeMap;
	}

	void OnImGUI();
};