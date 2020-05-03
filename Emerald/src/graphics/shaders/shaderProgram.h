class ShaderProgram {
private:
	GLuint m_handle = GL_INVALID_INDEX;
	GLuint m_globalUniformsBlockIndex = GL_INVALID_INDEX;
	int m_uniformCount = 0;
	vector<GLuint> m_attachedShaders;

public:
	ShaderProgram() {}
	~ShaderProgram() {
		DeleteProgram();
	}

	void CreateProgram() {
		ASSERT(!HasValidHandle(), "[~bShaders~x] Shader program already created");
		GL(m_handle = glCreateProgram());
	}

	void AttachShader(GLuint shader) {
		ASSERT(HasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		GL(glAttachShader(m_handle, shader));
		m_attachedShaders.push_back(shader);
	}

	void LinkAndValidate();

	void DeleteAttachedShaders() {
		for (GLuint shader : m_attachedShaders) {
			GL(glDeleteShader(shader));
		}
		m_attachedShaders.clear();
	}

	void DeleteProgram() {
		DeleteAttachedShaders();
		GL(glDeleteProgram(m_handle));
		m_handle = 0xffffffff;
	}

	bool HasValidHandle() {
		return m_handle != 0xffffffff;
	}

	GLuint GetHandle() {
		return m_handle;
	}

	void Bind() {
		GL(glUseProgram(m_handle));
	}

	void Unbind() {
		GL(glUseProgram(0));
	}

	int GetUniformCount() {
		return m_uniformCount;
	}

	uint GetUniformLocation(const String& location) {
		return glGetUniformLocation(m_handle, location.c_str());
	}

	struct UniformStruct {
		int uniformSize = 0;
		GLenum glType = 0;
		String name;
	};

	UniformStruct GetUniform(int index) {
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
};