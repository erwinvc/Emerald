#include "eepch.h"
#include "shader.h"
#include "shadInclude.h"
#include "utils/misc/glUtils.h"
#include "utils/system/fileSystem.h"
#include "graphics/core/renderer.h"
#include "shaderRegistry.h"

namespace emerald {
	Shader::Shader(const std::filesystem::path& filePath) : m_filePath(filePath), m_shaderProgram(nullptr), m_name(filePath.string()) {
		m_shaderProgram = load();
		if (!m_shaderProgram) {
			Log::error("[Shader] {} shader failed to compile", m_name.c_str());
		} else {
			gatherUniforms();
			m_valid = true;
		}
		//ShaderRegistry::registerShader(WeakRef<Shader>(this));
	}

	Shader::~Shader() {
		//ShaderRegistry::unregisterShader(this);
	}

	UniqueRef<ShaderProgram> Shader::load() {
		ParsedShader parsedShader;
		try {
			ShaderParser shaderParser;
			parsedShader = shaderParser.parseFile(m_filePath);
		} catch (std::exception ex) {
			Log::warn("[Shader] Failed to parse shader file {} [{}]", m_filePath.string(), ex.what());
			return nullptr;
		}

		m_name = parsedShader.programName;
		UniqueRef<ShaderProgram> shaderProgram = UniqueRef<ShaderProgram>::create();
		shaderProgram->createProgram();

		bool failed = false;

		if (!parsedShader.vertexSource.empty()) {
			failed |= addShaderToProgram(shaderProgram, parsedShader.vertexSource, GL_VERTEX_SHADER);
		}

		if (!parsedShader.fragmentSource.empty()) {
			failed |= addShaderToProgram(shaderProgram, parsedShader.fragmentSource, GL_FRAGMENT_SHADER);
		}

		if (!parsedShader.geometrySource.empty()) {
			failed |= addShaderToProgram(shaderProgram, parsedShader.geometrySource, GL_GEOMETRY_SHADER);
		}

		if (!parsedShader.tessControlSource.empty()) {
			failed |= addShaderToProgram(shaderProgram, parsedShader.tessControlSource, GL_TESS_CONTROL_SHADER);
		}

		if (!parsedShader.tessEvalSource.empty()) {
			failed |= addShaderToProgram(shaderProgram, parsedShader.tessEvalSource, GL_TESS_EVALUATION_SHADER);
		}

		if (failed) {
			return nullptr;
		} else {
			shaderProgram->linkAndValidate();
			shaderProgram->deleteAttachedShaders();
		}

		return std::move(shaderProgram);
	}

	uint32_t Shader::loadShader(const std::string& source, uint32_t type) {
		uint32_t shader = glCreateShader(type);
		std::string logOutput;
		//if (!Shadinclude::load(filePath, source, logOutput)) {
		//	Log::warn("[Shader] Failed to load {} xshader {} at {} [{}]", m_name.c_str(), GLUtils::shaderTypeToString(type), filePath.string(), logOutput.c_str());
		//	return 0xffffffff;
		//}

		const char* sourceCC = source.c_str();
		GL(glShaderSource(shader, 1, &sourceCC, nullptr));
		GL(glCompileShader(shader));

		GLint result;
		GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE) {
			GLint length;
			GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
			std::vector<char> error(length);
			GL(glGetShaderInfoLog(shader, length, &length, &error[0]));
			Log::warn("[Shader] Failed to compile {} {} shader\n{}", m_name.c_str(), GLUtils::shaderTypeToString(type), &error[0]);
			return 0xffffffff;
		} else {
			Log::info("[Shader] Compiled {} {} shader", m_name.c_str(), GLUtils::shaderTypeToString(type));
		}
		return shader;
	}

	bool Shader::addShaderToProgram(const UniqueRef<ShaderProgram>& program, const std::string& shader, uint32_t shaderType) {
		uint32_t shaderHandle = loadShader(shader, shaderType);
		if (shaderHandle == 0xffffffff) return true;
		program->attachShader(shaderHandle);
		return false;
	}

	void Shader::gatherUniforms() {
		m_uniformBuffers.clear();

		uint32_t handle = m_shaderProgram->handle();
		int count;
		GL(glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &count));

		uint32_t currentOffset = 0;

		for (int i = 0; i < count; i++) {
			int32_t nameSize;
			int32_t elementCount;
			uint32_t glType;
			char nameBuffer[64];
			GL(glGetActiveUniform(handle, i, 64, &nameSize, &elementCount, &glType, nameBuffer));
			std::string name = std::string(nameBuffer);
			if (elementCount > 1) name = name.substr(0, name.size() - 3); //Handle arrays

			uint32_t uniformLocation;
			GL(uniformLocation = (uint32_t)glGetUniformLocation(handle, name.c_str()));

			ShaderUniformType type = GLUtils::glTypeToShaderUniformType(glType);
			uint32_t size = GLUtils::getUniformSize(type);

			m_uniformBuffers[name] = ShaderUniform{
												name,
												GLUtils::glTypeToShaderUniformType(glType),
												elementCount > 1,
												size,
												(uint32_t)elementCount,
												currentOffset,
												uniformLocation
			};
			currentOffset += size * elementCount;
		}
	}

	bool Shader::reload() {
		bool success = false;
		UniqueRef<ShaderProgram> program = load();
		if (program) {
			m_shaderProgram = std::move(program);
			gatherUniforms();
			m_revision++;
			success = true;
		};
		return success;
	}

	ShaderUniform* Shader::getUniform(const std::string& name) {
		auto it = m_uniformBuffers.find(name);
		if (it != m_uniformBuffers.end()) {
			return &it->second;
		}
		return nullptr;
	}

	void Shader::setUniformInt(uint32_t location, const int32_t& value) {
		GL(glUniform1iv(location, 1, (GLint*)&value));
	}

	void Shader::setUniformInt(uint32_t location, uint32_t count, const int32_t* value) {
		GL(glUniform1iv(location, count, (GLint*)value));
	}

	void Shader::setUniformUInt(uint32_t location, const uint32_t& value) {
		GL(glUniform1uiv(location, 1, (GLuint*)&value));
	}

	void Shader::setUniformUInt(uint32_t location, uint32_t count, const uint32_t* value) {
		GL(glUniform1uiv(location, count, (GLuint*)value));
	}

	void Shader::setUniformFloat1(uint32_t location, const float& value) {
		GL(glUniform1fv(location, 1, (GLfloat*)&value));
	}

	void Shader::setUniformFloat1(uint32_t location, uint32_t count, const float* value) {
		GL(glUniform1fv(location, count, (GLfloat*)value));
	}

	void Shader::setUniformFloat2(uint32_t location, const glm::vec2& value) {
		GL(glUniform2fv(location, 1, (GLfloat*)&value));
	}

	void Shader::setUniformFloat2(uint32_t location, uint32_t count, const glm::vec2* value) {
		GL(glUniform2fv(location, count, (GLfloat*)value));
	}

	void Shader::setUniformFloat3(uint32_t location, const glm::vec3& value) {
		GL(glUniform3fv(location, 1, (GLfloat*)&value));
	}

	void Shader::setUniformFloat3(uint32_t location, uint32_t count, const glm::vec3* value) {
		GL(glUniform3fv(location, count, (GLfloat*)value));
	}

	void Shader::setUniformFloat4(uint32_t location, const glm::vec4& value) {
		GL(glUniform4fv(location, 1, (GLfloat*)&value));
	}

	void Shader::setUniformFloat4(uint32_t location, uint32_t count, const glm::vec4* value) {
		GL(glUniform4fv(location, count, (GLfloat*)value));
	}

	void Shader::setUniformMatrix3(uint32_t location, const glm::mat3& value) {
		GL(glUniformMatrix3fv(location, 1, GL_FALSE, (GLfloat*)&value));
	}

	void Shader::setUniformMatrix3(uint32_t location, uint32_t count, const glm::mat3* value) {
		GL(glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)value));
	}

	void Shader::setUniformMatrix4(uint32_t location, const glm::mat4& value) {
		GL(glUniformMatrix4fv(location, 1, GL_FALSE, (GLfloat*)&value));
	}

	void Shader::setUniformMatrix4(uint32_t location, uint32_t count, const glm::mat4* value) {
		GL(glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)value));
	}

	void Shader::bind() {
		m_shaderProgram->bind();
	}
}