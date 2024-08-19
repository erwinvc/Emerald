#include "eepch.h"
#include "shaderProgram.h"
#include "glError.h"
#include "../renderer.h"

namespace emerald {
	ShaderProgram::~ShaderProgram() {
		Renderer::submitFromAnyThread([this] {
			deleteAttachedShaders();
			GL(glDeleteProgram(m_handle));
		});
	}

	void ShaderProgram::createProgram() {
		ASSERT(!hasValidHandle(), "Shader program already created");
		GL(m_handle = glCreateProgram());
	}

	void ShaderProgram::attachShader(uint32_t shader) {
		ASSERT(hasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		GL(glAttachShader(m_handle, shader));
		GL(m_attachedShaders.push_back(shader));
	}

	void ShaderProgram::linkAndValidate() {
		ASSERT(hasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		GL(glLinkProgram(m_handle));
		GL(glValidateProgram(m_handle));

		getGlobalUniformBlock();

		deleteAttachedShaders();
	}

	void ShaderProgram::getGlobalUniformBlock() {
		m_globalUniformsBlockIndex = glGetUniformBlockIndex(m_handle, "GlobalUniforms");
		if (m_globalUniformsBlockIndex != GL_INVALID_INDEX) {
			GL(glUniformBlockBinding(m_handle, m_globalUniformsBlockIndex, 1));
		}
	}

	void ShaderProgram::deleteAttachedShaders() {
		for (uint32_t shader : m_attachedShaders) {
			GL(glDeleteShader(shader));
		}
		m_attachedShaders.clear();
	}

	void ShaderProgram::bind() {
		GL(glUseProgram(m_handle));
	}
}