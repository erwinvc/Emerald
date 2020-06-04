#include "stdafx.h"

ShaderProgram::~ShaderProgram() {
	DeleteProgram();
}

void ShaderProgram::CreateProgram() {
	ASSERT(!HasValidHandle(), "[~bShaders~x] Shader program already created");
	GL(m_handle = glCreateProgram());
}

void ShaderProgram::AttachShader(GLuint shader) {
	ASSERT(HasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

	GL(glAttachShader(m_handle, shader));
	m_attachedShaders.push_back(shader);
}

void ShaderProgram::LinkAndValidate() {
	ASSERT(HasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

	GL(glLinkProgram(m_handle));
	GL(glValidateProgram(m_handle));

	int count;
	GL(glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, &count));
	m_uniformCount = count;

	GL(m_globalUniformsBlockIndex = glGetUniformBlockIndex(m_handle, "GlobalUniforms"));
	if (m_globalUniformsBlockIndex != GL_INVALID_INDEX) {
		GL(glUniformBlockBinding(m_handle, m_globalUniformsBlockIndex, 1));
	}

	DeleteAttachedShaders();
}

void ShaderProgram::DeleteAttachedShaders() {
	for (GLuint shader : m_attachedShaders) {
		GL(glDeleteShader(shader));
	}
	m_attachedShaders.clear();
}

void ShaderProgram::DeleteProgram() {
	DeleteAttachedShaders();
	GL(glDeleteProgram(m_handle));
	m_handle = 0xffffffff;
}

void ShaderProgram::Bind() {
	GL(glUseProgram(m_handle));
}

void ShaderProgram::Unbind() {
	GL(glUseProgram(0));
}

uint ShaderProgram::GetUniformLocation(const String& location) {
	return glGetUniformLocation(m_handle, location.c_str());
}

ShaderProgram::UniformStruct ShaderProgram::GetUniform(uint32 index) {
	ASSERT(index <= m_uniformCount, "[~bShaders~x] Uniform index is greater than uniform count");
	GLsizei nameSize;
	GLint uniformSize;
	GLenum glType;
	GLchar nameBuffer[64];
	GL(glGetActiveUniform(m_handle, GLuint(index), 64, &nameSize, &uniformSize, &glType, nameBuffer));
	String name = nameBuffer;
	if (uniformSize > 1) name = name.substr(0, name.size() - 3);
	return { uniformSize, glType, name };
}