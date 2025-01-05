#pragma once
#include "graphics/core/renderer.h"
#include "shaderProgram.h"
#include "glad/glad.h"

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

		uint32_t handle() { return m_shaderProgram->handle(); }

		ShaderUniform* getUniform(const std::string& name);

		//void setUniform(const std::string& name, const void* value, uint32_t arrayCount = 1) {
		//	ShaderUniform* uniform = getUniform(name);
		//	if (!uniform) return;
		//
		//	uint32_t location = uniform->m_location;
		//	uint32_t count = (arrayCount == 0) ? uniform->m_count : arrayCount;
		//
		//	switch (uniform->m_type) {
		//		case ShaderUniformType::BOOL:
		//		{
		//			// Typically a bool is set as int for OpenGL
		//			const bool val = *static_cast<const bool*>(value);
		//			int32_t intVal = val ? 1 : 0;
		//			setUniformInt(location, count, intVal);
		//			break;
		//		}
		//		case ShaderUniformType::INT:
		//		{
		//			// If you only have a single int
		//			const int32_t val = *static_cast<const int32_t*>(value);
		//			setUniformInt(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::UINT:
		//		{
		//			const uint32_t val = *static_cast<const uint32_t*>(value);
		//			setUniformUInt(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::FLOAT:
		//		{
		//			const float val = *static_cast<const float*>(value);
		//			setUniformFloat1(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::VEC2:
		//		{
		//			const glm::vec2 val = *static_cast<const glm::vec2*>(value);
		//			setUniformFloat2(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::VEC3:
		//		{
		//			const glm::vec3 val = *static_cast<const glm::vec3*>(value);
		//			setUniformFloat3(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::VEC4:
		//		{
		//			const glm::vec4 val = *static_cast<const glm::vec4*>(value);
		//			setUniformFloat4(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::MAT3:
		//		{
		//			const glm::mat3 val = *static_cast<const glm::mat3*>(value);
		//			setUniformMatrix3(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::MAT4:
		//		{
		//			const glm::mat4 val = *static_cast<const glm::mat4*>(value);
		//			setUniformMatrix4(location, count, val);
		//			break;
		//		}
		//		case ShaderUniformType::NONE:
		//		default:
		//			Log::warn("Shader::setUniformValue() called with ShaderUniformType::NONE or unknown type for uniform '{}'.", uniform->m_name.c_str());
		//			break;
		//	}
		//}
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
		void internalSetUniform(GLFunc func, uint32_t location, uint32_t count, Args... args) {
			Renderer::submit([func, location, count, args...] {
				GL(func(location, count, args...));
			});
		}

		void setUniformInt(uint32_t location, const int32_t& value) {
			GL(glUniform1iv(location, 1, (GLint*)&value));
		}

		void setUniformInt(uint32_t location, uint32_t count, const int32_t* value) {
			GL(glUniform1iv(location, count, (GLint*)value));
		}

		void setUniformUInt(uint32_t location, const uint32_t& value) {
			GL(glUniform1uiv(location, 1, (GLuint*)&value));
		}

		void setUniformUInt(uint32_t location, uint32_t count, const uint32_t* value) {
			GL(glUniform1uiv(location, count, (GLuint*)value));
		}

		void setUniformFloat1(uint32_t location, const float& value) {
			GL(glUniform1fv(location, 1, (GLfloat*)&value));
		}

		void setUniformFloat1(uint32_t location, uint32_t count, const float* value) {
			GL(glUniform1fv(location, count, (GLfloat*)value));
		}

		void setUniformFloat2(uint32_t location, const glm::vec2& value) {
			GL(glUniform2fv(location, 1, (GLfloat*)&value));
		}

		void setUniformFloat2(uint32_t location, uint32_t count, const glm::vec2* value) {
			GL(glUniform2fv(location, count, (GLfloat*)value));
		}

		void setUniformFloat3(uint32_t location, const glm::vec3& value) {
			GL(glUniform3fv(location, 1, (GLfloat*)&value));
		}

		void setUniformFloat3(uint32_t location, uint32_t count, const glm::vec3* value) {
			GL(glUniform3fv(location, count, (GLfloat*)value));
		}

		void setUniformFloat4(uint32_t location, const glm::vec4& value) {
			GL(glUniform4fv(location, 1, (GLfloat*)&value));
		}

		void setUniformFloat4(uint32_t location, uint32_t count, const glm::vec4* value) {
			GL(glUniform4fv(location, count, (GLfloat*)value));
		}

		void setUniformMatrix3(uint32_t location, const glm::mat3& value) {
			GL(glUniformMatrix3fv(location, 1, GL_FALSE, (GLfloat*)&value));
		}

		void setUniformMatrix3(uint32_t location, uint32_t count, const glm::mat3* value) {
			GL(glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)value));
		}

		void setUniformMatrix4(uint32_t location, const glm::mat4& value) {
			GL(glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)&value));
		}

		void setUniformMatrix4(uint32_t location, uint32_t count, const glm::mat4* value) {
			GL(glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)value));
		}

		//void setUniformInt(uint32_t location, const int32_t& value);
		//void setUniformInt(uint32_t location, uint32_t count, const int32_t* value);
		//void setUniformUInt(uint32_t location, const uint32_t& value);
		//void setUniformUInt(uint32_t location, uint32_t count, const uint32_t* value);
		//void setUniformFloat1(uint32_t location, const float& value);
		//void setUniformFloat1(uint32_t location, uint32_t count, const float* value);
		//void setUniformFloat2(uint32_t location, const glm::vec2& value);
		//void setUniformFloat2(uint32_t location, uint32_t count, const glm::vec2* value);
		//void setUniformFloat3(uint32_t location, const glm::vec3& value);
		//void setUniformFloat3(uint32_t location, uint32_t count, const glm::vec3* value);
		//void setUniformFloat4(uint32_t location, const glm::vec4& value);
		//void setUniformFloat4(uint32_t location, uint32_t count, const glm::vec4* value);
		//void setUniformMatrix3(uint32_t location, const glm::mat3& value);
		//void setUniformMatrix3(uint32_t location, uint32_t count, const glm::mat3* value);
		//void setUniformMatrix4(uint32_t location, const glm::mat4& value);
		//void setUniformMatrix4(uint32_t location, uint32_t count, const glm::mat4* value);
	};
}