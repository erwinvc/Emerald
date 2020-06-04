class ShaderProgram {
private:
	struct UniformStruct {
		int uniformSize = 0;
		GLenum glType = 0;
		String name;
	};

	GLuint m_handle = GL_INVALID_INDEX;
	GLuint m_globalUniformsBlockIndex = GL_INVALID_INDEX;
	uint32 m_uniformCount = 0;
	vector<GLuint> m_attachedShaders;

	friend class ShaderUniformBuffer;
	
public:
	ShaderProgram() {}
	~ShaderProgram();
	
	void CreateProgram();
	void AttachShader(GLuint shader);
	void LinkAndValidate();
	void DeleteAttachedShaders();
	void DeleteProgram();
	void Bind();
	void Unbind();

	inline int GetUniformCount() { return m_uniformCount; }
	inline bool HasValidHandle() { return m_handle != 0xffffffff; }
	inline GLuint GetHandle() { return m_handle; }

	uint GetUniformLocation(const String& location);
	UniformStruct GetUniform(uint32 index);
};