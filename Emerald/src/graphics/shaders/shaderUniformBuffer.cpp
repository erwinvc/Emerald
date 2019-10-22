#include "stdafx.h"

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