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

	static uint ShaderUniformTypeToSize(ShaderUniformType type) {
		switch (type) {
			case ShaderUniformType::INT: return 4;
			case ShaderUniformType::FLOAT: return 4;
			case ShaderUniformType::VEC2: return 2 * 4;
			case ShaderUniformType::VEC3: return 3 * 4;
			case ShaderUniformType::VEC4: return 4 * 4;
			case ShaderUniformType::MAT4: return 16 * 4;
		}
		LOG_ERROR("[~bShaders~x] Unknow ShaderUniformType!");
		return 0;
	}

	static ShaderUniformType GLTypeToShaderUniformType(GLenum type) {
		switch (type) {
			case GL_BOOL_ARB:			return ShaderUniformType::INT;
			case GL_FLOAT: 				return ShaderUniformType::FLOAT;
			case GL_FLOAT_VEC2: 		return ShaderUniformType::VEC2;
			case GL_FLOAT_VEC3: 		return ShaderUniformType::VEC3;
			case GL_FLOAT_VEC4: 		return ShaderUniformType::VEC4;
			case GL_INT: 				return ShaderUniformType::INT;
			case GL_FLOAT_MAT4: 		return ShaderUniformType::MAT4;
			case GL_SAMPLER_1D: 		return ShaderUniformType::INT;
			case GL_SAMPLER_2D: 		return ShaderUniformType::INT;
			case GL_SAMPLER_3D: 		return ShaderUniformType::INT;
			case GL_SAMPLER_2D_ARRAY:	return ShaderUniformType::INT;
		}
		LOG_ERROR("[~bShaders~x] Unknow GL type ~10x%X~x!", type);
		return ShaderUniformType();
	}

	bool SetUniformLocal(uint offset, void* value, uint size, uint count) {
		bool same = memcmp(m_data + offset, value, size * count) == 0;
		memcpy(m_data + offset, value, size * count);
		return same;
	}

	void SetUniformGL(ShaderUniform& uniform) {
		uint location = uniform.GetLocation();
		uint offset = uniform.GetOffset();
		uint count = uniform.GetCount();
		switch (uniform.GetType()) {
			case ShaderUniformType::INT: SetGL(location, (int*)&m_data[offset], count); break;
			case ShaderUniformType::FLOAT: SetGL(location, (float*)&m_data[offset], count); break;
			case ShaderUniformType::VEC2: SetGL(location, (Vector2*)&m_data[offset], count); break;
			case ShaderUniformType::VEC3: SetGL(location, (Vector3*)&m_data[offset], count); break;
			case ShaderUniformType::VEC4: SetGL(location, (Vector4*)&m_data[offset], count); break;
			case ShaderUniformType::MAT4: SetGL(location, (Matrix4*)&m_data[offset], count); break;
		}
	}

	void SetGL(uint location, const int* value, uint count) { glUniform1iv(location, count, (int*)value); }
	//void SetGL(uint location, const bool* value, uint count) { glUniform1i(location, 1, value); }
	void SetGL(uint location, const Vector2I* value, uint count) { glUniform2iv(location, count, (int*)value); }
	void SetGL(uint location, const float* value, uint count) { glUniform1fv(location, count, (float*)value); }
	void SetGL(uint location, const Vector2* value, uint count) { glUniform2fv(location, count, (float*)value); }
	void SetGL(uint location, const Vector3* value, uint count) { glUniform3fv(location, count, (float*)value); }
	void SetGL(uint location, const Vector4* value, uint count) { glUniform4fv(location, count, (float*)value); }
	void SetGL(uint location, const Matrix4* value, uint count) { glUniformMatrix4fv(location, count, GL_TRUE, (float*)value); }

public:
	~ShaderUniformBuffer() {
		DeAllocate();
	}

	void AddUniform(const String& name, ShaderUniformType type, uint location, uint count, bool existsInShader) {
		ASSERT(!m_allocated, "[~bShaders~x] ShaderUniformBuffer memory has already been allocated!");
		uint size = ShaderUniformTypeToSize(type);
		m_uniforms.emplace(name, ShaderUniform(name, type, size, m_offset, count, location, existsInShader));
		m_offset += size * count;
	}

	void AddUniform(const String& name, GLenum glType, uint location, uint count, bool existsInShader) {
		AddUniform(name, GLTypeToShaderUniformType(glType), location, count, existsInShader);
	}

	void Allocate() {
		m_data = new byte[m_offset];
		for (uint i = 0; i < m_offset; i++) m_data[i] = 1;
		m_allocated = true;
	}

	void DeAllocate() {
		if (!m_allocated) return;
		delete[] m_data;
		m_allocated = false;
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

	void RegisterUniforms(ShaderProgram* shaderProgram, map<String, ShaderUniform>* oldMap = nullptr);

	void Reload(ShaderProgram* shaderProgram);

	void OnImGUI() {
		ImGui::Columns(2, NULL, true);
		int index = 0;
		for (auto it = m_uniforms.begin(); it != m_uniforms.end(); it++) {
			ShaderUniform& uniform = it->second;
			if (uniform.GetType() == ShaderUniformType::MAT4 || !uniform.m_existsInShader) continue;
			ImGui::Checkbox(Format_t("###%d", index++), &uniform.m_locked);
			ImGui::SameLine();
			ImGui::PushItemWidth(-130);
			switch (uniform.GetType()) {
				case ShaderUniformType::INT: if (ImGui::InputInt(uniform.GetName().c_str(), (int*)&m_data[uniform.GetOffset()])) { SetUniformGL(uniform); } break;
				case ShaderUniformType::FLOAT: if (ImGui::InputFloat(uniform.GetName().c_str(), (float*)&m_data[uniform.GetOffset()])) { SetUniformGL(uniform); } break;
				case ShaderUniformType::VEC2: if (ImGui::InputFloat2(uniform.GetName().c_str(), (float*)&m_data[uniform.GetOffset()])) { SetUniformGL(uniform); } break;
				case ShaderUniformType::VEC3: if (ImGui::InputFloat3(uniform.GetName().c_str(), (float*)&m_data[uniform.GetOffset()])) { SetUniformGL(uniform); } break;
				case ShaderUniformType::VEC4: if (ImGui::InputFloat4(uniform.GetName().c_str(), (float*)&m_data[uniform.GetOffset()])) { SetUniformGL(uniform); } break;
			}
			ImGui::PopItemWidth();
			ImGui::NextColumn();
		}
		ImGui::Columns(1);
	}
};