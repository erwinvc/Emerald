#include "eepch.h"
#include "shader.h"
#include "shadInclude.h"
#include "utils/misc/glUtils.h"
#include "utils/system/fileSystem.h"
#include "graphics/core/renderer.h"

namespace emerald {
	Shader::Shader(const std::string& name, const std::string& filePath, bool hasGeometry, bool hasTessellation) : m_shaderProgram(nullptr), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation), m_name(name), m_shaderPath(filePath) {
		Renderer::submit([instance = Ref<Shader>(this)]() mutable {
			instance->m_shaderProgram = instance->load();
			if (!instance->m_shaderProgram) Log::fatal("[Shader] {} failed to compile", instance->m_name.c_str());
			instance->gatherUniforms();
			instance->m_loaded = true;
		});
	}

	Shader::~Shader() {
		delete m_shaderProgram;
	}

	ShaderProgram* Shader::load() {
		ShaderProgram* shaderProgram = new ShaderProgram();
		shaderProgram->createProgram();

		bool failed = false;

		std::string vertexFile = m_shaderPath + ".vert";
		std::string fragFile = m_shaderPath + ".frag";
		failed |= addShaderToProgram(shaderProgram, vertexFile, GL_VERTEX_SHADER);
		failed |= addShaderToProgram(shaderProgram, fragFile, GL_FRAGMENT_SHADER);

		if (m_hasGeometry) {
			std::string geomFile = m_shaderPath + ".geom";
			failed |= addShaderToProgram(shaderProgram, geomFile, GL_GEOMETRY_SHADER);
		}

		if (m_hasTessellation) {
			std::string te = m_shaderPath + ".tese";
			std::string tc = m_shaderPath + ".tesc";
			failed |= addShaderToProgram(shaderProgram, te, GL_TESS_EVALUATION_SHADER);
			failed |= addShaderToProgram(shaderProgram, tc, GL_TESS_CONTROL_SHADER);
		}


		if (failed) {
			delete shaderProgram;
			shaderProgram = nullptr;
		} else {
			shaderProgram->linkAndValidate();
			shaderProgram->deleteAttachedShaders();
		}

		return shaderProgram;
	}

	uint32_t Shader::loadShader(const std::string& path, uint32_t type) {
		uint32_t shader = glCreateShader(type);
		if (!FileSystem::doesFileExist(path)) Log::warn("[Shader] {} {} at {} does not exist", m_name.c_str(), GLUtils::shaderTypeToString(type), path.c_str());

		std::string source;
		std::string logOutput;
		if (!Shadinclude::load(path, source, logOutput)) {
			Log::warn("[Shader] Failed to load {} xshader {} at {} [{}]", m_name.c_str(), GLUtils::shaderTypeToString(type), path.c_str(), logOutput.c_str());
			return 0xffffffff;
		}

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
			Log::warn("[Shader] Failed to compile {} {} shader with error: \n{}", m_name.c_str(), GLUtils::shaderTypeToString(type), &error[0]);
			return 0xffffffff;
		}
		Log::info("[Shader] Compiled {} {} shader", m_name.c_str(), GLUtils::shaderTypeToString(type));
		return shader;
	}

	bool Shader::addShaderToProgram(ShaderProgram* program, const std::string& shader, uint32_t shaderType) {
		uint32_t shaderHandle = loadShader(shader, shaderType);
		if (shaderHandle == 0xffffffff) return true;
		program->attachShader(shaderHandle);
		return false;
	}

	void Shader::gatherUniforms() {
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

	/*

	void Shader::setUniformInt(uint32_t location, uint32_t count, const int32_t& value) {
		Renderer::submit([=] {
			GL(glUniform1iv(location, count, (GLint*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1iv, location, count, (GLint*)value);
	}

	void Shader::setUniformInt(uint32_t location, uint32_t count, const std::vector<int32_t>& value) {
		Renderer::submit([=] {
			GL(glUniform1iv(location, count, (GLint*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1iv, location, count, (GLint*)value);
	}

	void Shader::setUniformUInt(uint32_t location, uint32_t count, const uint32_t& value) {
		Renderer::submit([=] {
			GL(glUniform1uiv(location, count, (GLuint*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1uiv, location, count, (GLuint*)value);
	}

	void Shader::setUniformUInt(uint32_t location, uint32_t count, const std::vector<uint32_t>& value) {
		Renderer::submit([=] {
			GL(glUniform1uiv(location, count, (GLuint*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1uiv, location, count, (GLuint*)value);
	}

	void Shader::setUniformFloat1(uint32_t location, uint32_t count, const float& value) {
		Renderer::submit([=] {
			GL(glUniform1fv(location, count, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat1(uint32_t location, uint32_t count, const std::vector<float>& value) {
		Renderer::submit([=] {
			GL(glUniform1fv(location, count, (GLfloat*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform1fv, location, count, (GLfloat*)value);
	}


	void Shader::setUniformFloat2(uint32_t location, uint32_t count, const glm::vec2& value) {
		Renderer::submit([=] {
			GL(glUniform2fv(location, count, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform2fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat2(uint32_t location, uint32_t count, const std::vector<glm::vec2>& value) {
		Renderer::submit([=] {
			GL(glUniform2fv(location, count, (GLfloat*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform2fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat3(uint32_t location, uint32_t count, const glm::vec3& value) {
		Renderer::submit([=] {
			GL(glUniform3fv(location, count, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform3fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat3(uint32_t location, uint32_t count, const std::vector<glm::vec3>& value) {
		Renderer::submit([=] {
			GL(glUniform3fv(location, count, (GLfloat*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform3fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat4(uint32_t location, uint32_t count, const glm::vec4& value) {
		Renderer::submit([=] {
			GL(glUniform4fv(location, count, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniform4fv, location, count, (GLfloat*)value);
	}

	void Shader::setUniformFloat4(uint32_t location, uint32_t count, const std::vector<glm::vec4>& value) {
		Renderer::submit([=] {
			GL(glUniform4fv(location, count, (GLfloat*)value.data()));
		});
	}

	void Shader::setUniformMatrix3(uint32_t location, uint32_t count, const glm::mat3& value) {
		Renderer::submit([=] {
			GL(glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniformMatrix3fv, location, count, GL_FALSE, value);
	}

	void Shader::setUniformMatrix3(uint32_t location, uint32_t count, const std::vector<glm::mat3>& value) {
		Renderer::submit([=] {
			GL(glUniformMatrix3fv(location, count, GL_FALSE, (GLfloat*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniformMatrix3fv, location, count, GL_FALSE, value);
	}

	void Shader::setUniformMatrix4(uint32_t location, uint32_t count, const glm::mat4& value) {
		Renderer::submit([=] {
			GL(glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)&value));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniformMatrix4fv, location, count, GL_FALSE, value);
	}

	void Shader::setUniformMatrix4(uint32_t location, uint32_t count, const std::vector<glm::mat4>& value) {
		Renderer::submit([=] {
			GL(glUniformMatrix4fv(location, count, GL_FALSE, (GLfloat*)value.data()));
			//GL(func(location, count, args...));
		});
		//setUniform(glUniformMatrix4fv, location, count, GL_FALSE, value);
	}

	*/

	void Shader::reload() {
		//Ref<Shader> instance = this;
		//Renderer::submit([instance]() mutable {
		//	ShaderProgram* program = instance->load();
		//	if (program) {
		//		delete instance->m_shaderProgram;
		//		instance->m_shaderProgram = program;
		//		instance->m_uniformBuffer.reload(instance->m_shaderProgram);
		//	};
		//});
	}


	void Shader::bind() {
		Renderer::submit([instance = Ref<Shader>(this)] {
			instance->m_shaderProgram->bind();
		});
	}
}