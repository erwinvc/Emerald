#include "eepch.h"
#include "shaderUniformBuffer.h"
#include "assert.h"

namespace emerald {
	void ShaderUniformBuffer::initialize(const ShaderProgram* shaderProgram) {
		uint32_t handle = shaderProgram->handle();
		int count;
		glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count);
		m_uniformCount = count;

		for (uint32_t i = 0; i < m_uniformCount; i++) {
			int32_t nameSize;
			int32_t uniformSize;
			uint32_t glType;
			char nameBuffer[64];
			glGetActiveUniform(handle, i, 64, &nameSize, &uniformSize, &glType, nameBuffer);
			std::string name = std::string(nameBuffer);
			if (uniformSize > 1) name = name.substr(0, name.size() - 3);

			ShaderUniform uniform = { name, (uint32_t)glGetUniformLocation(handle, name.c_str()), glTypeToShaderUniformType(glType) };
			m_uniforms.emplace(name, uniform);
		}
	}

	void ShaderUniformBuffer::reload(const ShaderProgram* shaderProgram) {
		m_uniforms.clear();
		m_uniformCount = 0;
		initialize(shaderProgram);
	}

}
