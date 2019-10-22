#pragma once

class ShaderProgram {
private:
	GLuint m_handle = 0xffffffff;
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

	void LinkAndValidate() {
		ASSERT(HasValidHandle(), "[~bShaders~x] Invalid shader program handle. Did you call CreateProgram?");

		GL(glLinkProgram(m_handle));
		GL(glValidateProgram(m_handle));

		int count;
		GL(glGetProgramiv(m_handle, GL_ACTIVE_UNIFORMS, &count));
		m_uniformCount = count;

		DeleteAttachedShaders();
	}

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

class Shader {
private:
	ShaderProgram* m_shaderProgram;

	bool m_hasGeometry;
	bool m_hasTessellation;
	String m_name;
	String m_file;

	ShaderUniformBuffer m_uniformBuffer;

	GLuint LoadShader(const String& path, GLuint type) {
		GL(uint shader = glCreateShader(type));
		if(!FileSystem::DoesFileExist(path)) LOG_WARN("[~bShaders~x] ~1%s ~xshader ~1%s ~xat ~1%s does not exist", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str());
		String source = FileSystem::ReadFile(path);
		if (source.empty()) {
			LOG_WARN("[~bShaders~x] Failed to load ~1%s ~xshader ~1%s ~xat ~1%s", m_name.c_str(), GLUtils::ShaderTypeToString(type), path.c_str());
			return 0xffffffff;
		}
		String_t sourceCC = source.c_str();
		GL(glShaderSource(shader, 1, &sourceCC, 0));
		GL(glCompileShader(shader));

		GLint result;
		GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE) {
			GLint length;
			GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
			vector<char> error(length);
			GL(glGetShaderInfoLog(shader, length, &length, &error[0]));
			LOG_WARN("[~bShaders~x] Failed to compile %s %s shader with error: \n%s", m_name.c_str(), GLUtils::ShaderTypeToString(type), &error[0]);
			return 0xffffffff;
		}
		LOG("[~bShaders~x] Compiled ~1%s~x %s", m_name.c_str(), GLUtils::ShaderTypeToString(type));
		return shader;
	}

	bool AddShaderToProgram(ShaderProgram* program, const String& shader, GLuint shaderType) {
		GLuint shaderHandle = LoadShader(shader, shaderType);
		if (shaderHandle == 0xffffffff) return true;
		program->AttachShader(shaderHandle);
		return false;
	}

	ShaderProgram* Load() {
		ShaderProgram* shaderProgram = new ShaderProgram();
		shaderProgram->CreateProgram();

		bool failed = false;

		String vertexFile = m_file + ".vert";
		String fragFile = m_file + ".frag";
		failed |= AddShaderToProgram(shaderProgram, vertexFile, GL_VERTEX_SHADER);
		failed |= AddShaderToProgram(shaderProgram, fragFile, GL_FRAGMENT_SHADER);

		if (m_hasGeometry) {
			String geomFile = m_file + ".geom";
			failed |= AddShaderToProgram(shaderProgram, geomFile, GL_GEOMETRY_SHADER);
		}

		if (m_hasTessellation) {
			String te = m_file + ".tese";
			String tc = m_file + ".tesc";
			failed |= AddShaderToProgram(shaderProgram, te, GL_TESS_EVALUATION_SHADER);
			failed |= AddShaderToProgram(shaderProgram, tc, GL_TESS_CONTROL_SHADER);
		}


		if (failed) {
			delete shaderProgram;
			shaderProgram = nullptr;
		} else {
			shaderProgram->LinkAndValidate();
			shaderProgram->DeleteAttachedShaders();
		}

		return shaderProgram;
	}

	Shader(const String& name, const String& file, bool hasGeometry = false, bool hasTessellation = false) : m_shaderProgram(nullptr), m_hasGeometry(hasGeometry), m_hasTessellation(hasTessellation), m_name(name), m_file(file) {
		m_shaderProgram = Load();
		if (!m_shaderProgram) LOG_ERROR("[~bShaders~x] ~1%s~x shader failed to compile", name.c_str());
		m_uniformBuffer.RegisterUniforms(m_shaderProgram);
	}

	~Shader() {
		delete m_shaderProgram;
	}

	friend class ShaderManager;
public:

	template<typename T>
	void Set(const String_t location, const T* value, uint count) {
		m_uniformBuffer.Set(location, value, count);
	}

	template<typename T>
	void Set(const String_t location, const T& value) {
		m_uniformBuffer.Set(location, value);
	}

	template <>
	void Set<bool>(const String_t location, const bool& value) {
		m_uniformBuffer.Set(location, (int)value);
	}

	void Reload() {
		ShaderProgram* program = Load();
		if (program) {
			delete m_shaderProgram;
			m_shaderProgram = program;
			m_uniformBuffer.Reload(m_shaderProgram);
		};
	}

	void Bind() {
		m_shaderProgram->Bind();
	}

	void Unbind() {
		m_shaderProgram->Unbind();
	}

	void OnImGUI();
};