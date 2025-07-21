#pragma once
#include "graphics/core/renderer.h"
#include "shaderProgram.h"
#include "shaderParser.h"
#include "engine/assets/core/asset.h"

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
		uint32_t m_size = 0; //size in bytes
		uint32_t m_count = 0; //number of elements
		uint32_t m_offset = 0; //offset from previous element in bytes
		uint32_t m_location = 0;
	};

	class Shader : public Asset {
	public:
		Shader(const std::filesystem::path& filePath);
		~Shader();

		const std::string& getName() { return m_name; }
		const std::unordered_map<std::string, ShaderUniform>& getUniformBuffers() const { return m_uniformBuffers; }

		bool isValid() const { return m_valid; }
		void bind();

		uint32_t handle() { return m_shaderProgram->handle(); }

		ShaderUniform* getUniform(const std::string& name);
		bool reload() override;
		uint32_t getRevision() const { return m_revision; }

	private:
		friend class Material;

		std::string m_name;
		std::filesystem::path m_filePath;
		UniqueRef<ShaderProgram> m_shaderProgram;
		std::unordered_map<std::string, ShaderUniform> m_uniformBuffers;
		bool m_valid = false;
		uint32_t m_revision = 0;

		UniqueRef<ShaderProgram> load();
		uint32_t loadShader(const std::string& source, uint32_t type);
		bool addShaderToProgram(const UniqueRef<ShaderProgram>& program, const std::string& filePath, uint32_t shaderType);
		void gatherUniforms();

		void setUniformInt(uint32_t location, const int32_t& value);
		void setUniformInt(uint32_t location, uint32_t count, const int32_t* value);
		void setUniformUInt(uint32_t location, const uint32_t& value);
		void setUniformUInt(uint32_t location, uint32_t count, const uint32_t* value);
		void setUniformFloat1(uint32_t location, const float& value);
		void setUniformFloat1(uint32_t location, uint32_t count, const float* value);
		void setUniformFloat2(uint32_t location, const glm::vec2& value);
		void setUniformFloat2(uint32_t location, uint32_t count, const glm::vec2* value);
		void setUniformFloat3(uint32_t location, const glm::vec3& value);
		void setUniformFloat3(uint32_t location, uint32_t count, const glm::vec3* value);
		void setUniformFloat4(uint32_t location, const glm::vec4& value);
		void setUniformFloat4(uint32_t location, uint32_t count, const glm::vec4* value);
		void setUniformMatrix3(uint32_t location, const glm::mat3& value);
		void setUniformMatrix3(uint32_t location, uint32_t count, const glm::mat3* value);
		void setUniformMatrix4(uint32_t location, const glm::mat4& value);
		void setUniformMatrix4(uint32_t location, uint32_t count, const glm::mat4* value);
	};
}