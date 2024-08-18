#include "eepch.h"
#include "shaderUniformBuffer.h"
#include "assert.h"

namespace emerald {

	ShaderUniformType ShaderUniformBuffer::glTypeToShaderUniformType(uint32_t type) {
		switch (type) {
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
		Log::error("Unknow GL type {}", type);
		return ShaderUniformType();
	}

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


	template<typename T>
	inline void ShaderUniformBuffer::set(const std::string& name, const T& value) {
		set<T>(name, &value, 1);
	}

	template<typename T>
	inline void ShaderUniformBuffer::set(const std::string& name, const T* value, uint32_t count) {
		ASSERT(m_uniforms.find(name) != m_uniforms.end(), "UniformBuffer does not contain uniform");

		ShaderUniform& uniform = m_uniforms.at(name);
		switch (uniform.m_type) {
			case ShaderUniformType::INT:	glUniform1iv(uniform.m_location, count, (int*)value); break;
			case ShaderUniformType::FLOAT:	glUniform1fv(uniform.m_location, count, (float*)value); break;
			case ShaderUniformType::VEC2:	glUniform2fv(uniform.m_location, count, (float*)value); break;
			case ShaderUniformType::VEC3:	glUniform3fv(uniform.m_location, count, (float*)value); break;
			case ShaderUniformType::VEC4:	glUniform4fv(uniform.m_location, count, (float*)value); break;
			case ShaderUniformType::MAT4:	glUniformMatrix4fv(uniform.m_location, count, GL_FALSE, (float*)value); break;
		}
	}


}
