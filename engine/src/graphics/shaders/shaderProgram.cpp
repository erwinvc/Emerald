#include "eepch.h"
#include "shaderProgram.h"

namespace emerald {
	ShaderProgram::~ShaderProgram() {
		deleteProgram();
	}

	void ShaderProgram::createProgram() {
		ASSERT(!hasValidHandle(), "Shader program already created");
		m_handle = glCreateProgram();
	}

	void ShaderProgram::attachShader(uint32_t shader) {
		ASSERT(hasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		glAttachShader(m_handle, shader);
		m_attachedShaders.push_back(shader);
	}

	void ShaderProgram::linkAndValidate() {
		ASSERT(hasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		glLinkProgram(m_handle);
		glValidateProgram(m_handle);

		getGlobalUniformBlock();

		deleteAttachedShaders();
	}

	void ShaderProgram::getGlobalUniformBlock() {
		m_globalUniformsBlockIndex = glGetUniformBlockIndex(m_handle, "GlobalUniforms");
		if (m_globalUniformsBlockIndex != GL_INVALID_INDEX) {
			glUniformBlockBinding(m_handle, m_globalUniformsBlockIndex, 1);
		}
	}

	void ShaderProgram::deleteAttachedShaders() {
		for (uint32_t shader : m_attachedShaders) {
			glDeleteShader(shader);
		}
		m_attachedShaders.clear();
	}

	void ShaderProgram::deleteProgram() {
		deleteAttachedShaders();
		glDeleteProgram(m_handle);
		m_handle = 0xffffffff;
	}

	void ShaderProgram::bind() {
		glUseProgram(m_handle);
	}
}