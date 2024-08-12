#pragma once
#include "shaderProgram.h"

namespace emerald {
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

	struct ShaderUniform {
		std::string m_name;
		uint32_t m_location;
		ShaderUniformType m_type;
	};

	class ShaderUniformBuffer {
	private:
		uint32_t m_uniformCount;
		std::unordered_map<std::string, ShaderUniform> m_uniforms;

		ShaderUniformType glTypeToShaderUniformType(uint32_t type) {
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
	public:
		void initialize(const ShaderProgram* shaderProgram);

		template<typename T>
		void set(const std::string& name, const T& value) {
			set<T>(name, &value, 1);
		}

		template<typename T>
		void set(const std::string& name, const T* value, uint32_t count) {
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

		void reload(const ShaderProgram* m_shaderProgram);
	};
}

