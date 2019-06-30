#pragma once

class Shader {
	GLuint m_shaderID;

	bool m_hasGeometry;
	String m_name;
	String m_vertex;
	String m_geometry;
	String m_fragment;

	map<String, GLuint> m_uniforms;

	GLuint LoadShader(String path, GLuint type, bool disableLog) {
		GL(uint shader = glCreateShader(type));
		String source = FileSystem::ReadFile(path);
		if (!disableLog && source.empty()) {
			LOG_ERROR("[~bShaders~x] Failed to load %s shader %s", m_name.c_str(), GLUtils::ShaderTypeToString(type));
			return -1;
		}
		String_t sourceCC = source.c_str();
		GL(glShaderSource(shader, 1, &sourceCC, 0));
		GL(glCompileShader(shader));

		GLint result;
		GL(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE) {
			if (!disableLog) {
				GLint length;
				GL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
				vector<char> error(length);
				GL(glGetShaderInfoLog(shader, length, &length, &error[0]));
				LOG_ERROR("[~bShaders~x] Failed to compile %s %s shader with error: \n%s", m_name.c_str(), GLUtils::ShaderTypeToString(type), &error[0]);
			}
			GL(glDeleteShader(shader));
			return -1;
		}
		if (!disableLog)LOG("[~bShaders~x] Compiled ~1%s~x %s", m_name.c_str(), GLUtils::ShaderTypeToString(type));
		return shader;
	}

	GLuint Load(bool reload = false) {
		GL(uint program = glCreateProgram());
		uint vertex = LoadShader(m_vertex, GL_VERTEX_SHADER, reload);
		uint fragment = LoadShader(m_fragment, GL_FRAGMENT_SHADER, reload);
		uint geometry = 0;
		if (m_hasGeometry) geometry = LoadShader(m_geometry, GL_GEOMETRY_SHADER, reload);


		if (vertex == -1 || geometry == -1 || fragment == -1) {
			GL(glDeleteProgram(program));
			return -1;
		}

		GL(glAttachShader(program, vertex));
		if (m_hasGeometry) {
			GL(glAttachShader(program, geometry));
		}
		GL(glAttachShader(program, fragment));
		GL(glLinkProgram(program));
		GL(glValidateProgram(program));

		GL(glDeleteShader(vertex));
		GL(glDeleteShader(fragment));

		return program;
	}

	uint GetUniform(const String_t location) {
		static int timer;
		auto it = m_uniforms.find(location);
		if (it == m_uniforms.end()) {
			//LOG_TIMED(250, "[~bSh3kiaders~x] Failed to find Uniform %s:%s", m_name, location);
			return -1;
		}
		return it->second;
	}
public:
	Shader() {}
	Shader(String name, String vertex, String geometry, String fragment) : m_shaderID(0), m_hasGeometry(true), m_name(name), m_vertex(vertex), m_geometry(geometry), m_fragment(fragment) {
		m_shaderID = Load();
		RegisterUniforms();
	}

	Shader(String name, String vertex, String fragment) : m_shaderID(0), m_hasGeometry(false), m_name(name), m_vertex(vertex), m_geometry(""), m_fragment(fragment) {
		m_shaderID = Load();
		RegisterUniforms();
	}
	~Shader() { GL(glDeleteProgram(m_shaderID)); }

	void Set(const String_t location, const int value) { glUniform1i(GetUniform(location), value); }
	void Set(const String_t location, const int x, const int y) { glUniform2i(GetUniform(location), x, y); }
	void Set(const String_t location, const float value) { glUniform1f(GetUniform(location), value); }
	void Set(const String_t location, const float x, const float y) { glUniform2f(GetUniform(location), x, y); }
	void Set(const String_t location, const Color& color) { glUniform4f(GetUniform(location), color.R, color.G, color.B, color.A); }
	void Set(const String_t location, float x, float y, float z) { glUniform3f(GetUniform(location), x, y, z); }
	void Set(const String_t location, const Matrix4& matrix) { glUniformMatrix4fv(GetUniform(location), 1, GL_TRUE, matrix.elements); }
	void Set(const String_t location, const Rect& rect) { glUniform4f(GetUniform(location), rect.m_position.x, rect.m_position.y, rect.m_size.x, rect.m_size.y); }
	void Set(const String_t location, const Vector4& vector) { glUniform4f(GetUniform(location), vector.x, vector.y, vector.z, vector.w); }
	void Set(const String_t location, const Vector3& vector) { glUniform3f(GetUniform(location), vector.x, vector.y, vector.z); }
	void Set(const String_t location, const Vector2& vector) { glUniform2f(GetUniform(location), vector.x, vector.y); }
	//void SetArray(const String_t location, const Vector3* vectors, int count) { glUniform3fv(GetUniform(location), count, (float*)vectors); }

	void Reload() {
		uint program = Load(true);
		if (program != -1) {
			GL(glDeleteProgram(m_shaderID));
			m_shaderID = program;
		}
	}

	inline void Bind() {
		GL(glUseProgram(m_shaderID));
	}

	inline void Unbind() {
		GL(glUseProgram(0));
	}

protected:
	void AddUniform(String_t uniform) {
		GL(int uniformLocation = glGetUniformLocation(m_shaderID, uniform));
		if (uniformLocation == 0xffffffff) LOG("[~bShaders~x] Failed to find uniform~1 %s~x in the ~1%s ~xshader", uniform, m_name);
		m_uniforms.emplace(uniform, uniformLocation);
		//LOG("%s in %s = %d", uniform, m_name, uniformLocation);
	}
	void RegisterUniforms() {
		if (m_shaderID == -1) return;
		GLint count;
		GLint size;
		GLenum type;
		const GLsizei bufSize = 64;
		GLchar name[bufSize];
		GLsizei length;
		GL(glGetProgramiv(m_shaderID, GL_ACTIVE_UNIFORMS, &count));
		for (int i = 0; i < count; i++) {
			GL(glGetActiveUniform(m_shaderID, (GLuint)i, bufSize, &length, &size, &type, name));
			if (size > 1) {
				String temp = name;
				temp = temp.substr(0, temp.find("[", 0));
				for (int j = 0; j < size; j++) AddUniform(Format("%s[%d]", temp.c_str(), j).c_str());
			} else AddUniform(name);
			//AddUniform(name);
		}
	}
};