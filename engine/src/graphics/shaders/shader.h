#pragma once
#include "../renderer.h"
#include "glError.h"
#include "shaderProgram.h"

namespace emerald {
	enum class ShaderUniformType {
		NONE,
		BOOL,
		INT,
		UINT,
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT3,
		MAT4,
	};

	struct ShaderUniform {
		std::string m_name;
		ShaderUniformType m_type = ShaderUniformType::NONE;
		bool m_isArray = false;
		uint32_t m_size = 0; //Size in bytes
		uint32_t m_count = 0; //Number of elements
		uint32_t m_offset = 0; //Offset from previous element in bytes
		uint32_t m_location = 0; 
	};

	class Shader : public RefCounted {
	public:
		Shader(const std::string& name, const std::string& filePath, bool hasGeometry = false, bool hasTessellation = false);
		~Shader();

		const std::string& getName() { return m_name; }
		const std::unordered_map<std::string, ShaderUniform>& getUniformBuffers() const { return m_uniformBuffers; }

		bool isLoaded() const { return m_loaded; }
		void reload();
		void bind();

		uint32_t handle(){ return m_shaderProgram->handle(); }

	private:
		friend class Material;

		std::string m_name;
		std::string m_shaderPath;
		ShaderProgram* m_shaderProgram;
		std::unordered_map<std::string, ShaderUniform> m_uniformBuffers;
		bool m_loaded = false;
		bool m_hasGeometry = false;
		bool m_hasTessellation = false;

		ShaderProgram* load();
		uint32_t loadShader(const std::string& path, uint32_t type);
		bool addShaderToProgram(ShaderProgram* program, const std::string& shader, uint32_t shaderType);
		void gatherUniforms();

		template<typename GLFunc, typename... Args>
		void setUniform(GLFunc func, uint32_t location, uint32_t count, Args... args) {
			Renderer::submit([func, location, count, args...] {
				GL(func(location, count, args...));
			});
		}

		void setUniformInt(uint32_t location, uint32_t count, const int32_t* value);
		void setUniformUInt(uint32_t location, uint32_t count, const uint32_t* value);
		void setUniformFloat1(uint32_t location, uint32_t count, const float* value);
		void setUniformFloat2(uint32_t location, uint32_t count, const float* value);
		void setUniformFloat3(uint32_t location, uint32_t count, const float* value);
		void setUniformFloat4(uint32_t location, uint32_t count, const float* value);
		void setUniformMatrix3(uint32_t location, uint32_t count, const float* value);
		void setUniformMatrix4(uint32_t location, uint32_t count, const float* value);
	};
}