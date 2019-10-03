#include "stdafx.h"

void ShaderUniformBuffer::RegisterUniforms(ShaderProgram* shaderProgram) {
	for (int i = 0; i < shaderProgram->GetUniformCount(); i++) {
		ShaderProgram::UniformStruct& uniform = shaderProgram->GetUniform(i);
		uint location = shaderProgram->GetUniformLocation(uniform.name.c_str());
		AddUniform(uniform.name, uniform.glType, location, uniform.uniformSize);
	}
	Allocate();
}

void ShaderUniformBuffer::Reload(ShaderProgram* shaderProgram) {
	map<String, ShaderUniform> tempUniforms = m_uniforms;
	m_uniforms.clear();
	byte* tempBytes = new byte[m_offset];
	memcpy(tempBytes, m_data, m_offset);
	DeAllocate();

	RegisterUniforms(shaderProgram);

	for (auto it = tempUniforms.begin(); it != tempUniforms.end(); it++) {
		void* data = tempBytes + it->second.GetOffset();
		auto res = m_uniforms.find(it->first.c_str());
		if (res != m_uniforms.end()) {
			memcpy(m_data + res->second.GetOffset(), data, it->second.GetSize() * it->second.GetCount());
			it->second.m_locked = res->second.m_locked;
		}
	}

	for (auto it = m_uniforms.begin(); it != m_uniforms.end(); it++) {
		ShaderUniform& uniform = it->second;
		SetUniformGL(uniform);
	}

	delete[] tempBytes;
}