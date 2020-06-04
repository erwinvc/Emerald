#include "stdafx.h"

ShaderUniformBuffer::~ShaderUniformBuffer() {
	DeAllocate();
}

uint ShaderUniformBuffer::ShaderUniformTypeToSize(ShaderUniformType type) {
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

ShaderUniformType ShaderUniformBuffer::GLTypeToShaderUniformType(GLenum type) {
	switch (type) {
		case GL_BOOL_ARB:			return ShaderUniformType::INT;
		case GL_FLOAT: 				return ShaderUniformType::FLOAT;
		case GL_FLOAT_VEC2: 		return ShaderUniformType::VEC2;
		case GL_FLOAT_VEC3: 		return ShaderUniformType::VEC3;
		case GL_FLOAT_VEC4: 		return ShaderUniformType::VEC4;
		case GL_INT: 				return ShaderUniformType::INT;
		case GL_FLOAT_MAT4: 		return ShaderUniformType::MAT4;
		case GL_SAMPLER_CUBE: 		return ShaderUniformType::INT;
		case GL_SAMPLER_1D: 		return ShaderUniformType::INT;
		case GL_SAMPLER_2D: 		return ShaderUniformType::INT;
		case GL_SAMPLER_3D: 		return ShaderUniformType::INT;
		case GL_SAMPLER_2D_ARRAY:	return ShaderUniformType::INT;
	}
	LOG_ERROR("[~bShaders~x] Unknow GL type ~10x%X~x!", type);
	return ShaderUniformType();
}

void ShaderUniformBuffer::AddUniform(const String& name, ShaderUniformType type, uint location, uint count, bool existsInShader) {
	ASSERT(!m_allocated, "[~bShaders~x] ShaderUniformBuffer memory has already been allocated!");
	uint size = ShaderUniformTypeToSize(type);
	m_uniforms.emplace(name, ShaderUniform(name, type, size, m_offset, count, location, existsInShader));
	m_offset += size * count;
}

void ShaderUniformBuffer::AddUniform(const String& name, GLenum glType, uint location, uint count, bool existsInShader) {
	AddUniform(name, GLTypeToShaderUniformType(glType), location, count, existsInShader);
}

bool ShaderUniformBuffer::SetUniformLocal(uint offset, void* value, uint size, uint count) {
	bool same = memcmp(m_data + offset, value, size * count) == 0;
	memcpy(m_data + offset, value, size * count);
	return same;
}

void ShaderUniformBuffer::SetUniformGL(ShaderUniform& uniform) {
	uint location = uniform.GetLocation();
	uint offset = uniform.GetOffset();
	uint count = uniform.GetCount();
	switch (uniform.GetType()) {
		case ShaderUniformType::INT: SetGL(location, (int*)&m_data[offset], count); break;
		case ShaderUniformType::FLOAT: SetGL(location, (float*)&m_data[offset], count); break;
		case ShaderUniformType::VEC2: SetGL(location, (glm::vec2*) & m_data[offset], count); break;
		case ShaderUniformType::VEC3: SetGL(location, (glm::vec3*) & m_data[offset], count); break;
		case ShaderUniformType::VEC4: SetGL(location, (glm::vec4*) & m_data[offset], count); break;
		case ShaderUniformType::MAT4: SetGL(location, (glm::mat4*) & m_data[offset], count); break;
	}
}

void ShaderUniformBuffer::Allocate() {
	m_data = new byte[m_offset];
	for (uint i = 0; i < m_offset; i++) m_data[i] = 1;
	m_allocated = true;
}

void ShaderUniformBuffer::DeAllocate() {
	if (!m_allocated) return;
	delete[] m_data;
	m_allocated = false;
}

void ShaderUniformBuffer::RegisterUniforms(ShaderProgram* shaderProgram, map<String, ShaderUniform>* oldMap) {
	m_offset = 0;
	for (int i = 0; i < shaderProgram->GetUniformCount(); i++) {
		ShaderProgram::UniformStruct& uniform = shaderProgram->GetUniform(i);
		uint location = shaderProgram->GetUniformLocation(uniform.name.c_str());
		AddUniform(uniform.name, uniform.glType, location, uniform.uniformSize, true);
	}

	if (oldMap) {
		for (auto it = oldMap->begin(); it != oldMap->end(); it++) {
			auto uniform = m_uniforms.find(it->first.c_str());
			if (uniform == m_uniforms.end()) {
				AddUniform(it->second.GetName(), it->second.GetType(), it->second.GetLocation(), it->second.GetSize(), false);
			}
		}
	}
	Allocate();
}

void ShaderUniformBuffer::Reload(ShaderProgram* shaderProgram) {
	shaderProgram->Bind();
	map<String, ShaderUniform> tempUniforms = m_uniforms;
	m_uniforms.clear();
	auto size = sizeof(m_data);
	byte* tempBytes = new byte[m_offset];
	memcpy(tempBytes, m_data, m_offset);
	DeAllocate();

	RegisterUniforms(shaderProgram, &tempUniforms);

	for (auto tempUniform = tempUniforms.begin(); tempUniform != tempUniforms.end(); tempUniform++) {
		void* tempData = tempBytes + tempUniform->second.GetOffset();
		auto uniform = m_uniforms.find(tempUniform->first.c_str());
		if (uniform != m_uniforms.end()) {
			SetUniformLocal(uniform->second.GetOffset(), tempData, uniform->second.GetSize(), uniform->second.GetCount());
			SetUniformGL(uniform->second);
			tempUniform->second.m_locked = uniform->second.m_locked;
		}
	}

	delete[] tempBytes;
}

uint ShaderUniformBuffer::GetUniformLocation(const String_t location) {
	auto res = m_uniforms.find(location);
	if (res == m_uniforms.end()) return 0xFFFFFFFF;
	return res->second.GetLocation();
}

void ShaderUniformBuffer::OnImGUI() {
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