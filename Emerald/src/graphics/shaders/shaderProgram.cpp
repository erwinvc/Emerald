#include "stdafx.h"

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