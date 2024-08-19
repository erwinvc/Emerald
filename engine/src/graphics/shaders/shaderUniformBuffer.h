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

		ShaderUniformType glTypeToShaderUniformType(uint32_t type);
	public:
		ShaderUniformBuffer() : m_uniformCount(0) {}

		void initialize(const ShaderProgram* shaderProgram);

		template<typename T>
		void set(const std::string& name, const T& value);

		template<typename T>
		void set(const std::string& name, const T* value, uint32_t count);

		void reload(const ShaderProgram* m_shaderProgram);
	};
}

