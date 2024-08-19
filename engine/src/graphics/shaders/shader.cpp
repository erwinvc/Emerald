#include "eepch.h"
#include "stdIncl.h"
#include "shader.h"
#include "shadInclude.h"
#include "util/glUtils.h"
#include "util/fileSystem.h"
#include "../renderer.h"
#include "glError.h"

namespace emerald {
	Shader::Shader(const std::string& name, const std::string& filePath, bool hasGeometry, bool hasTessellation) : m_shaderProgram(nullptr), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation), m_name(name), m_shaderPath(filePath) {
		Ref<Shader> instance = this;
		Renderer::submit([instance]() mutable {
			instance->m_shaderProgram = instance->load();
			if (!instance->m_shaderProgram) Log::fatal("[Shader] {} failed to compile", instance->m_name.c_str());
			instance->m_uniformBuffer.initialize(instance->m_shaderProgram);
		});
	}

	Shader::~Shader() {
		delete m_shaderProgram;
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

	void Shader::reload() {
		Ref<Shader> instance = this;
		Renderer::submit([instance]() mutable {
			ShaderProgram* program = instance->load();
			if (program) {
				delete instance->m_shaderProgram;
				instance->m_shaderProgram = program;
				instance->m_uniformBuffer.reload(instance->m_shaderProgram);
			};
		});
	}

	void Shader::bind() {
		Ref<Shader> instance = this;
		Renderer::submit([instance] {
			instance->m_shaderProgram->bind();
		});
	}
}